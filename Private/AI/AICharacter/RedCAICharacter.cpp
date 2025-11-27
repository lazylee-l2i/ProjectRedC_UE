// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter/RedCAICharacter.h"
#include "AI/AIController/RedCAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/DataAsset/AIDataAsset.h"
#include "GAS/Attributes/RedCCombatSet.h"
#include "GAS/Attributes/RedCHealthSet.h"
#include "Perception/AISense_Sight.h"
#include "Types/RedCGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Characters/DefaultMontageData.h"
#include "Core/Systems/RedCAssetManager.h"
#include "Libraries/RedCFunctionLibrary.h"
#include "Perception/AIPerceptionTypes.h"
#include "AI/Spawn/RedCAISpawnActor.h"
#include "Characters/RedCHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Types/RedCLogChannels.h"


ARedCAICharacter::ARedCAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	StimuliSource->bAutoRegister = false;

	AbilitySystemComponent = CreateDefaultSubobject<URedCAIAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true); 
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	GetMesh()->SetIsReplicated(true);
	
	AIComAttrSet = CreateDefaultSubobject<URedCCombatSet>(TEXT("RedCCombatSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AIComAttrSet);

	AIHpAttrSet = CreateDefaultSubobject<URedCHealthSet>(TEXT("RedCHealthSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AIHpAttrSet);
	
	
	// 기본값 설정
	bPerceptionDelegatesBound = false;
	bIsDead = false;
	bIsActiveInWorld = false;
	CurrentWaypointIndex = 0;
	
	
	
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		//RVO 끄기 
		MoveComp->bUseRVOAvoidance = false;  
		MoveComp->SetAvoidanceEnabled(false);   

		MoveComp->MaxWalkSpeed = (MoveSpeed > 150.f) ? MoveSpeed : 300.f;
		MoveComp->BrakingFrictionFactor = 0.5f;     
		MoveComp->bUseSeparateBrakingFriction = true;
	}
}

const FName ARedCAICharacter::BBKey_StopMoveByTag(TEXT("bStopMoveByTag"));

void ARedCAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 컨트롤러에 팀 ID 전달
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->SetGenericTeamId(FGenericTeamId(TeamID));
	}

	if (HasAuthority())
	{
		UpdateCachedTeamId(TeamID);
	}

	if (AbilitySystemComponent)
	{
		const URedCHealthSet* HealthSet = AbilitySystemComponent->GetSet<URedCHealthSet>();
		if (HealthSet)
		{
			// Health Attribute 변경 시 호출될 델리게이트 바인딩
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetHealthAttribute())
				.AddUObject(this, &ARedCAICharacter::OnHealthChanged);
		}

		//태그 들어오면 안움직이게
		AbilitySystemComponent->RegisterGameplayTagEvent(
		PlayerStateTags::Player_State_Combat_KnockedDown,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ARedCAICharacter::OnStopMoveTagChanged);
	}
	FString CharacterName;
	CharacterTag.ToString().Split(TEXT("."), nullptr, &CharacterName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	const FGameplayTag CharacterAssetTag = URedCFunctionLibrary::FindChildTagByName(CharacterName, AssetTags::Asset_Character_Montage);
	if (!CharacterAssetTag.IsValid())
	{
		return;
	}
	MontageData = URedCAssetManager::GetAssetByTag<UDefaultMontageData>(CharacterAssetTag);
	
	if (MontageData)
	{
		GetUpMontage = MontageData->MontageMap.Find(PlayerStateTags::Player_State_KnockedDownRecover)->Get();
		if (!GetUpMontage)
		{
			return;
		}
		DeathMontage = MontageData->MontageMap.Find(AiAbilityTags::AI_Ability_Death)->Get();
		if (!DeathMontage)
		{
			return;
		}
	}
	
	//태그 들어오면 멈추게 만드는거
	for (const FGameplayTag& Tag : StopMoveTagList)
	{
		GetAbilitySystemComponent()->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ARedCAICharacter::OnStopMoveTagChanged);
	}

	//몽타주 실행시 멈추게 만드는거
	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		AnimInst->OnMontageStarted.AddDynamic(this, &ARedCAICharacter::OnMontageStarted);
		AnimInst->OnMontageEnded.AddDynamic(this, &ARedCAICharacter::OnMontageEnded);
	}
	
	ConfigureOverheadHealthWidget();
	SetTeamColorMaterial();
}

void ARedCAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!AbilitySystemComponent) return;

	EnsureASCActorInfo();
	
	//ASC 초기화
	if(bFirstSpawn)
	{
		AbilitySystemComponent->InitAbilityActorInfo(NewController, this);
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
	
	// 능력/효과 부여
	if (HasAuthority() && AIConfig &&!bAbilitiesInitialized)
	{
		if (!bAbilitiesInitialized && bFirstSpawn)
		{
			InitializeAbilitiesAndEffects();
			ApplyAIStatsFromConfig(true, true);
			bAbilitiesInitialized = true;
		}
	}
	
	// 컨트롤러 설정 및 BT 실행은 마지막에
	if (ARedCAIController* AICon = Cast<ARedCAIController>(NewController))
	{
		AICon->SetGenericTeamId(GetGenericTeamId());
		
		if (BehaviorTreeAsset)
		{
			UBlackboardComponent* BB = nullptr;
			AICon->UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BB);

			// 초기화 시 어그로 해제 보장
			if (BB)
			{
				BB->ClearValue(TEXT("TargetActor"));
				BB->SetValueAsBool(TEXT("bIsAggroed"), false);
			}

			// ASC가 완전히 세팅된 이후 BT 실행
			AICon->RunBehaviorTree(BehaviorTreeAsset);

			if (UBrainComponent* Brain = AICon->GetBrainComponent())
			{
				Brain->StartLogic();
			}
		}
	}
}

void ARedCAICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARedCAICharacter, TeamID);
}

void ARedCAICharacter::ActivateAI()
{
	if (bIsActiveInWorld) return;
	
	GenerationId++; 
	bIsActiveInWorld = true;
	bIsDead = false;
	bReturn = false;
	
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);

	// 캡슐/메시 충돌 복구 (풀 복귀 시 완전 비활성했던 거 되살림)
	if (UCapsuleComponent* Cap = GetCapsuleComponent())
	{
		uint8 TeamNumber = GetCharacterTeamId_UINT8();
		if (TeamNumber == 0)
		{
			GetCapsuleComponent()->SetCollisionProfileName(FName("RedTeamProfile"));
		}
		else
		{
			GetCapsuleComponent()->SetCollisionProfileName(FName("BlueTeamProfile"));
		}
		Cap->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Cap->SetGenerateOverlapEvents(true);                                
	}
	
	if (USkeletalMeshComponent* Skel = GetMesh())
	{
		
		Skel->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		if (UAnimInstance* Anim = Skel->GetAnimInstance())
		{
			Anim->StopAllMontages(0.f);
			Anim->ResetDynamics(ETeleportType::TeleportPhysics);
			Anim->InitializeAnimation();
		}
		
		Skel->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
		Skel->SetGenerateOverlapEvents(false);
		Skel->SetComponentTickEnabled(true);
		Skel->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately(); // 이전 잔여 이동 제거
		MoveComp->SetComponentTickEnabled(true); 
		MoveComp->SetMovementMode(MOVE_Walking);
		MoveComp->MaxWalkSpeed = MoveSpeed > 0 ? MoveSpeed : 300.f;
	}
	
	// Stimuli 켜기
	RegisterStimuli();
	
	// Controller 확보
	
	if (!HasAuthority()) return;
	{
		AController* Ctrl = GetController();

		// 이미 붙어 있으면 그대로 사용
		if (Ctrl)
		{
			if (Ctrl->GetPawn() != this)
				Ctrl->Possess(this);
			CachedController = Ctrl; // 캐싱
		}
		else if (CachedController && IsValid(CachedController))
		{
			// 캐싱된 컨트롤러 재사용
			CachedController->Possess(this);
		}
		else
		{
			// 없으면 최초 생성
			static TSubclassOf<AController> CachedBPControllerClass = nullptr;
			if (!CachedBPControllerClass)
			{
				static const TCHAR* BPControllerPath = TEXT("/Game/Blueprint/AI/BP_RedCAIController.BP_RedCAIController_C");  //bp 실제경로
				CachedBPControllerClass = StaticLoadClass(AController::StaticClass(), nullptr, BPControllerPath);
			}

			if (CachedBPControllerClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = nullptr;
				SpawnParams.Instigator = nullptr;
        
				AController* NewCtrl = GetWorld()->SpawnActor<AController>(
									CachedBPControllerClass, GetActorLocation(), GetActorRotation(), SpawnParams);


				if (NewCtrl)
				{
					NewCtrl->Possess(this);
					CachedController = NewCtrl; 
							
				}
			}
		}
		
		if (AbilitySystemComponent)
		{
			AController* ValidCtrl = GetController();
			const bool bNeedReinit =
				!AbilitySystemComponent->AbilityActorInfo.IsValid() ||
				AbilitySystemComponent->AbilityActorInfo->OwnerActor.Get() != ValidCtrl ||
				AbilitySystemComponent->AbilityActorInfo->AvatarActor.Get() != this;

			if (bNeedReinit && ValidCtrl)
			{
				AbilitySystemComponent->InitAbilityActorInfo(ValidCtrl, this);
				AbilitySystemComponent->RefreshAbilityActorInfo();
			}
			else
			{
				EnsureASCActorInfo();
			}
			
			if (AIConfig)
			{
				AbilitySystemComponent->ClearAllAbilities();
				AbilitySystemComponent->RemoveAllGameplayCues();
				AbilitySystemComponent->RemoveActiveEffectsWithTags(FGameplayTagContainer());

				InitializeAbilitiesAndEffects();
				ApplyAIStatsFromConfig(false, true);
			}
		}
	}
	///
	// 컨트롤러 초기화
	///
	if(ARedCAIController* AICon = Cast<ARedCAIController>(GetController()))
	{
		AICon->SetGenericTeamId(TeamID);

		// Perception 리스너 즉시 갱신 (아군/적 분류 캐시 갱신)
		if (UAIPerceptionComponent* PC = AICon->GetPerceptionComponent())
		{
			PC->ForgetAll();
			PC->RequestStimuliListenerUpdate();
		}

		if (BehaviorTreeAsset)
		{
			AICon->RunBehaviorTree(BehaviorTreeAsset);
		}
		
		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			if (!Brain->IsRunning())
			{
				Brain->RestartLogic();
			}
		}
		
		// CrowdFollowing 활성화
		if (UCrowdFollowingComponent* Crowd = Cast<UCrowdFollowingComponent>(AICon->GetPathFollowingComponent()))
		{
			Crowd->SetCrowdSeparation(true);          // 개체 간 분리
			Crowd->SetCrowdSeparationWeight(5.0f);    // 1.0~5.0 사이 튜닝
			Crowd->SetCrowdObstacleAvoidance(true);   // 장애물 회피
			Crowd->SetCrowdAnticipateTurns(true);     // 회전 예측
			Crowd->SetCrowdOptimizeTopology(true);    // 국소 경로 최적화
		}
		
		//  PathFollowing 보장
		if (UPathFollowingComponent* PFC = AICon->GetPathFollowingComponent())
		{
			PFC->Activate();
			PFC->SetComponentTickEnabled(true);
		}
	
		// Blackboard 초기값 세팅
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->ClearValue(TEXT("TargetActor"));
			BB->SetValueAsBool(TEXT("bIsAggroed"), false);
			BB->SetValueAsBool(TEXT("bStopMoveByTag"), false);
		}
	}
	// 클라이언트에서 Controller 늦게 붙는 경우
	if (!HasAuthority() && AbilitySystemComponent)
	{
		GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if (AbilitySystemComponent && GetController())
			{
				AbilitySystemComponent->InitAbilityActorInfo(GetController(), this);
			}
		}));
	}

	if (ARedCAIController* AICon = Cast<ARedCAIController>(GetController()))
	{
		
		if (UAIPerceptionComponent* Perception = AICon->GetPerceptionComponent())
		{
			Perception->RequestStimuliListenerUpdate();
		}

		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("AI Re-Activated"));
			Brain->RestartLogic(); 
		}

		SwitchHitBoxStateToStanding();
		
		if (UAIPerceptionComponent* Perception = AICon->GetPerceptionComponent())
		{
			TArray<AActor*> Sensed;
			Perception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Sensed);

			if (Sensed.Num() > 0)
			{
				APawn* MyPawn = AICon->GetPawn();
				AActor* Best = nullptr;
				float BestDistSq = TNumericLimits<float>::Max();

				for (AActor* Actor : Sensed)
				{
					if (!IsValid(Actor) || Actor == this) continue;

					// 팀 체크: 적만 처리
					if (AICon->GetTeamAttitudeTowards(*Actor) != ETeamAttitude::Hostile)
						continue;

					// 가장 가까운 적 추려두기
					const float DistSq = (MyPawn ? FVector::DistSquared(MyPawn->GetActorLocation(), Actor->GetActorLocation())
												 : FVector::DistSquared(GetActorLocation(), Actor->GetActorLocation()));
					if (DistSq < BestDistSq)
					{
						BestDistSq = DistSq;
						Best = Actor;
					}
					
					const UAISense_Sight& SightSense = *UAISense_Sight::StaticClass()->GetDefaultObject<UAISense_Sight>();
					
					FAIStimulus Stim(
						SightSense,                       // 감각 타입 (필수)
						1.f,                               // Strength
						Actor->GetActorLocation(),         // Stimulus Location
						GetActorLocation(),                // Receiver Location
						FAIStimulus::SensingSucceeded,     // 감지 성공 상태
						NAME_None                          // 태그 (생략 가능)
					);
					AICon->TargetPerceptionUpdate(Actor, Stim);
				}

				// Blackboard에 아직 Target이 비어있으면 가장 가까운 적을 즉시 세팅
				if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
				{
					if (!BB->GetValueAsObject(TEXT("TargetActor")) && Best)
					{
						AICon->SetCurrentTarget(Best);
						BB->SetValueAsBool(TEXT("bIsAggroed"), true);
					}
				}
			}
		}
	}
	bFirstSpawn = false;
}

void ARedCAICharacter::DeactivateAI()
{
	if (!bIsActiveInWorld) return; // 이미 비활성화된 경우 스킵
	bIsActiveInWorld = false;
	bReturn = false;

	GetWorldTimerManager().ClearTimer(AutoDeathTimerHandle);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	
	// AI 이동/브레인 중단
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UPathFollowingComponent* PFC = AICon->GetPathFollowingComponent())
		{
			PFC->AbortMove(*this, FPathFollowingResultFlags::ForcedScript);
		}
		AICon->StopMovement();
		
		if (AICon->GetPawn() == this)
		{
			AICon->UnPossess();
		}
	}
	
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	// Stimuli 끄기
	UnRegisterStimuli();
	
	//  캡슐/메시 충돌과 오버랩 모두 내리기
	if (UCapsuleComponent* Cap = GetCapsuleComponent())
	{
		Cap->SetCollisionProfileName(NAME_None);      
		Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision);         
		Cap->SetCollisionResponseToAllChannels(ECR_Ignore);                   
		Cap->SetGenerateOverlapEvents(false);                                 
	}

	//스켈레톤 끄기
	if (USkeletalMeshComponent* Skel = GetMesh())
	{
		Skel->SetCollisionEnabled(ECollisionEnabled::NoCollision);   
		Skel->SetGenerateOverlapEvents(false);  
	}
	
	// 움직임 컴포넌트 비활성화
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopActiveMovement();
		MoveComp->DisableMovement();                        
		MoveComp->SetComponentTickEnabled(false);
		MoveComp->SetAvoidanceEnabled(false);    
	}
	
	//웨이포인트 리셋
	CurrentWaypointIndex = 0;
	// 델리게이트 바인딩 플래그 리셋
	bPerceptionDelegatesBound = false;

	if (AbilitySystemComponent)
	{
		// 돌고 있던 모든 능력 강제 종료
		AbilitySystemComponent->CancelAllAbilities();
		
		// 모든 이펙트/쿨다운/큐 제거
		AbilitySystemComponent->RemoveAllGameplayCues();
		AbilitySystemComponent->RemoveActiveEffectsWithTags(FGameplayTagContainer());
		
		FGameplayTagContainer ClearTags;
		ClearTags.AddTag(PlayerStateTags::Player_State_Combat_KnockedDown);
		ClearTags.AddTag(PlayerStateTags::Player_State_Death);
		ClearTags.AddTag(AiAbilityTags::AI_Ability_Death);
		ClearTags.AddTag(SharedTags::Event_Shared_HitReact_Heavy);

		AbilitySystemComponent->RemoveLooseGameplayTags(ClearTags);
		AbilitySystemComponent->RemoveActiveEffectsWithTags(ClearTags);
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(ClearTags);
		
	}
	
	// 메시에 걸려있던 모든 애님 몽타주도 강제 정지
	if (USkeletalMeshComponent* Skel = GetMesh())
	{
		if (UAnimInstance* Anim = Skel->GetAnimInstance())
		{
			Anim->StopAllMontages(0.f);
			Anim->ResetDynamics(ETeleportType::TeleportPhysics);
			Anim->InitializeAnimation();
		}
	}

	if (ARedCAIController* AICon = Cast<ARedCAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->ClearValue(TEXT("TargetActor"));
			BB->SetValueAsBool(TEXT("bIsAggroed"), false);
		}
	}
	// 지하 주차: 혹시 남은 상호작용을 원천 차단 (네비/시야/오버랩 영향 최소화)
	static const FVector PoolParkLoc(0.f, 0.f, -500000.f);
	SetActorLocation(PoolParkLoc, false, nullptr, ETeleportType::TeleportPhysics);

	bIsMontagePlaying = false;
}

void ARedCAICharacter::SpawnDefaultController()
{
	
}

void ARedCAICharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	const float NewHealth = Data.NewValue;
	
	if (!HasAuthority())
		return;
	if (bIsDead || bReturn)
	{
		return;
	}
	
	// 체력이 0 이하일 때 사망 함수 호출
	if (NewHealth <= 0.f)
	{
		UE_LOG(LogRedC, Warning, TEXT("[AI:%s] Health <= 0 -> Call OnDead()"), *GetName());
		OnDead(); 
	}
	
	// 데스 어빌리티 태그로 활성
	FGameplayTagContainer DeathAbilityTag;
	DeathAbilityTag.AddTag(AiAbilityTags::AI_Ability_Death);
	const bool bActivated = AbilitySystemComponent->TryActivateAbilitiesByTag(DeathAbilityTag);
}

void ARedCAICharacter::OnDead()
{
	if (bIsDead || bReturn) 
	{
		return;
	}
	
	bIsDead = true;
	bReturn = false;

	SwitchHitBoxStateToKnockdown();	
	
	if (AController* Ctrl = GetController())
	{
		if (AAIController* AICtrl = Cast<AAIController>(Ctrl))
		{
			if (UBrainComponent* Brain = AICtrl->GetBrainComponent())
			{
				Brain->StopLogic(TEXT("AI Died"));
			}
		}
	}
	if (HasAuthority() && IsValid(DeathMontage))
	{
		MulticastPlayMontage(DeathMontage);
	}
	else
	{
		UE_LOG(LogRedCAI, Warning, TEXT("AI Death Montage is not valid. %s"), *GetName());
	}
	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &ARedCAICharacter::RequestReturnToPool, 3.0f, false);
}

void ARedCAICharacter::RequestReturnToPool()
{
	if (bReturn) return;
	bReturn = true;
	if (OwnerSpawnerWeak.IsValid())
	{
		ARedCAISpawnActor* Spawner = OwnerSpawnerWeak.Get();
		Spawner->ReturnAIToPool(this);
		
		UE_LOG(LogTemp, Warning, TEXT("[AI:%s] RequestReturnToPool"), *GetName());
	}
}


void ARedCAICharacter::Revive(const TArray<ARedC_AISpawnPoint*>& InWaypoints)
{
	Waypoints = InWaypoints;
	
	bIsDead = false;
	
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer RemoveTags;
		RemoveTags.AddTag(AiAbilityTags::AI_Ability_Death);
		RemoveTags.AddTag(PlayerStateTags::Player_State_Death);
		RemoveTags.AddTag(PlayerStateTags::Player_State_Combat_KnockedDown);
		
		AbilitySystemComponent->RemoveLooseGameplayTags(RemoveTags);
		AbilitySystemComponent->RemoveActiveEffectsWithTags(RemoveTags);
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(RemoveTags);

		for (const FGameplayTag& Tag : RemoveTags)
		{
			bool bStillHas = AbilitySystemComponent->HasMatchingGameplayTag(Tag);
			UE_LOG(LogRedC, Warning, TEXT("[Revive] Tag %s still active? %s"), *Tag.ToString(), bStillHas ? TEXT("TRUE") : TEXT("FALSE"));
		}
	}
	SwitchHitBoxStateToStanding();
	
	 if (HasAuthority() && IsValid(GetUpMontage))
	 {
	 	MulticastPlayMontage(GetUpMontage);
	 }
	
	ActivateAI();
}

bool ARedCAICharacter::IsAlive() const
{
	return !bIsDead && bIsActiveInWorld; 
}

void ARedCAICharacter::InitializeAbilitiesAndEffects()
{
	if (!AbilitySystemComponent || !AIConfig) return;
	
	// 모든 능력을 하나의 배열로 수집
	TArray<TSubclassOf<UGameplayAbility>> AllAbilities;
	
	// BaseAbilities 추가
	for (auto& Pair : AIConfig->BaseAbilities)
	{
		if (Pair.Value)
			AllAbilities.Add(Pair.Value);
	}
	
	// 한 번에 초기화
	AbilitySystemComponent->InitializeAbilitiesAndEffectsSafe(AllAbilities, AIConfig->InitialEffects);
}

void ARedCAICharacter::ApplyAIStatsFromConfig(bool bApplyStartStats, bool bApplyCurrentHealth)
{
	if (!HasAuthority() || !AbilitySystemComponent || !AIConfig) return;

	
	const TSubclassOf<UGameplayEffect>StartStatGE   =AIConfig->StartStatGE;
	const TSubclassOf<UGameplayEffect>CurrentStatGE = AIConfig->CurrentStatGE;
	
	if (bApplyStartStats && StartStatGE)
	{
		FGameplayEffectSpecHandle SpecH = AbilitySystemComponent->MakeOutgoingSpec
		(StartStatGE, /*Level=*/1.f, AbilitySystemComponent->MakeEffectContext());

		if (SpecH.IsValid() && SpecH.Data.IsValid())
		{
			SpecH.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_MaxHealth,   AIConfig->AIData.MaxHealth);
			SpecH.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_AttackPower, AIConfig->AIData.AttackPower);
			SpecH.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_Defense,     AIConfig->AIData.Defense);
			
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecH.Data.Get());
		}
	}
	if (bApplyCurrentHealth && CurrentStatGE)
	{
		FGameplayEffectSpecHandle SpecH = AbilitySystemComponent->MakeOutgoingSpec(CurrentStatGE, /*Level=*/1.f, AbilitySystemComponent->MakeEffectContext());
		if (SpecH.IsValid() && SpecH.Data.IsValid())
		{
			SpecH.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_Health, AIConfig->AIData.CurrentHealth);
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecH.Data.Get());
		}
	}
}

void ARedCAICharacter::EnsureASCActorInfo()
{
	if (!AbilitySystemComponent) return;

	AController* Ctrl = GetController();
	const bool bNeed =
		!AbilitySystemComponent->AbilityActorInfo.IsValid() ||
		AbilitySystemComponent->AbilityActorInfo->OwnerActor.Get()  != Ctrl ||
		AbilitySystemComponent->AbilityActorInfo->AvatarActor.Get() != this;

	if (bNeed && Ctrl)
	{
		AbilitySystemComponent->InitAbilityActorInfo(Ctrl, this);
		AbilitySystemComponent->RefreshAbilityActorInfo();
		
	}
}

void ARedCAICharacter::OnDeath()
{
	
}

void ARedCAICharacter::SetCharacterStopStateInBlackBoard(bool bStop)
{
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();

		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->SetValueAsBool(TEXT("bStopMoveByTag"),bStop);
		}
	}
}

void ARedCAICharacter::InitAbilityActorInfo()
{
	
}

UAbilitySystemComponent* ARedCAICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ARedCAICharacter::ShouldStopMove() const
{
	// HeavyHitReact 몽타주 중이거나
	if (bIsMontagePlaying)
		return true;

	// StopMove 태그가 하나라도 남아 있으면
	if (AbilitySystemComponent)
	{
		for (const FGameplayTag& Tag : StopMoveTagList)
		{
			if (AbilitySystemComponent->HasMatchingGameplayTag(Tag))
				return true;
		}
	}

	// 위 조건이 모두 아니면 이동 가능
	return false;
}

void ARedCAICharacter::OnMontageStarted(UAnimMontage* Montage)
{
	if (!bIsActiveInWorld || bIsDead)
		return;
	
	if (Montage && Montage->GetFName() == BlockMoveMontageName)
	{
		bIsMontagePlaying = true;

		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			AICon->StopMovement();

			if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
			{
				BB->SetValueAsBool(TEXT("bStopMoveByTag"),true);
			}
		}
	}
}
void ARedCAICharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage)
		return;

	// 넘어지는 몽타주 끝났을 때 (멈춤 유지 판단)
	if (Montage->GetFName() == BlockMoveMontageName)
	{
		bIsMontagePlaying = false;

		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
			{
				if (ShouldStopMove())
					BB->SetValueAsBool(TEXT("bStopMoveByTag"), true);
			}
		}

		if (bIsDead)
		{
			return;
		}
		
		if (HasAuthority() && IsValid(GetUpMontage))
		{
			MulticastPlayMontage(GetUpMontage);
		}
	}

	// 일어나는 몽타주 끝났을 때 (이동 복귀)
	else if (Montage == GetUpMontage)
	{
		// 이제 이동 재개 허용
		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
			{
				BB->SetValueAsBool(TEXT("bStopMoveByTag"), false);
			}
		}
	}
}

 void ARedCAICharacter::RegisterStimuli()
 {
 	if (StimuliSource && !StimuliSource->IsRegistered())
 	{
 		StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
 		StimuliSource->RegisterWithPerceptionSystem();
 	}
 }
 void ARedCAICharacter::UnRegisterStimuli()
 {
 	if (StimuliSource && StimuliSource->IsRegistered())
 	{
 		StimuliSource->UnregisterFromPerceptionSystem();
 	}
}

void ARedCAICharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	EnsureASCActorInfo();
}

void ARedCAICharacter::OnStopMoveTagChanged(const FGameplayTag InTag, int32 NewCount)
{
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (ShouldStopMove())
		{
			//  이동 중단 (MoveTo / MoveToWaypoint 전부 포함)
			AICon->StopMovement();

			//  블랙보드에 Stop 상태 반영
			if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
			{
				BB->SetValueAsBool(TEXT("bStopMoveByTag"), true);
			}
		}
		else
		{
			// 태그 제거 시 다시 false로 변경
			if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
			{
				BB->SetValueAsBool(TEXT("bStopMoveByTag"), false);
			}
		}
	}
}

void ARedCAICharacter::SetTeamColorMaterial()
{
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (!MeshComponent)
	{
		return;
	}

	check(AIConfig);

	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (const ARedCHeroCharacter* Character = PC->GetPawn<ARedCHeroCharacter>())
		{
			FAITeamMaterials TeamMaterials = AIConfig->TeamMaterials[IsCharacterSameTeam(Character)];

			if (TeamMaterials.BodyMaterial && TeamMaterials.LensMaterial)
			{
				MeshComponent->SetMaterial(0, TeamMaterials.BodyMaterial);
				MeshComponent->SetMaterial(1, TeamMaterials.LensMaterial);
			}
		}
	}
}
