// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIController/RedCAIController.h"

#include "AI/AICharacter/RedCAICharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Navigation/PathFollowingComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h" 


ARedCAIController::ARedCAIController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))  
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->SightRadius = 700.0f;
	SightConfig->LoseSightRadius = 1000.0f;
	SightConfig->SetMaxAge(1.0f);
	SightConfig->PeripheralVisionAngleDegrees = 180.0f;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	
	TargetBlackBoardKeyName = "TargetActor";
}

void ARedCAIController::InitializeAndActivateAI(int32 InTeamID, UBehaviorTree* BT)
{

	ARedCAICharacter* AIChar = Cast<ARedCAICharacter>(GetPawn());
	if (!AIChar|| !BT) return;

	SetGenericTeamId(FGenericTeamId(InTeamID));

	UBlackboardComponent* BB = nullptr;
	
	//BT실행
	UseBlackboard(BT->BlackboardAsset, BB);
	if (BT)
	{
		RunBehaviorTree(BT);
	}
	
	// PathFollowing 활성화 보장
	if (UPathFollowingComponent* PFC = GetPathFollowingComponent())
	{
		PFC->Activate();
		PFC->SetComponentTickEnabled(true);
	}
	
	if (!AIChar->bPerceptionDelegatesBound)
	{
		BindPerceptionDelegates();
		AIChar->bPerceptionDelegatesBound = true;
	}
	
	//감각 활성화
	AIChar->RegisterStimuli();

	if (AIChar->StimuliSource)
	{
		AIChar->StimuliSource->UnregisterFromPerceptionSystem();
		AIChar->StimuliSource->RegisterWithPerceptionSystem();
	}
	// // Stimuli 재등록 타이머
	// if (AIChar->StimuliTimerHandle.IsValid())
	// {
	// 	GetWorldTimerManager().ClearTimer(AIChar->StimuliTimerHandle);
	// }
	// GetWorldTimerManager().SetTimer(AIChar->StimuliTimerHandle, this, &ARedCAIController::ReRegisterStimuli, 1.f, false);
}

void ARedCAIController::ReRegisterStimuli()
{
	if (ARedCAICharacter* AIChar = Cast<ARedCAICharacter>(GetPawn()))
	{
		if (AIChar->StimuliSource)
		{
			AIChar->StimuliSource->UnregisterFromPerceptionSystem();
			AIChar->StimuliSource->RegisterWithPerceptionSystem();
		}
	}
}


void ARedCAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ARedCAICharacter* AIChar = Cast<ARedCAICharacter>(InPawn);
	if (!AIChar) return;

	UnbindTowerDestroyedIfNeeded();
	
	SetGenericTeamId(FGenericTeamId(AIChar->TeamID));
	//델리게이트 설정
	if (!AIChar->bPerceptionDelegatesBound)
	{
		BindPerceptionDelegates();
		AIChar->bPerceptionDelegatesBound = true;
	}
	AIChar->RegisterStimuli();
	
	if (AIChar->BehaviorTreeAsset)
	{
		UBlackboardComponent* BB = nullptr;
		UseBlackboard(AIChar->BehaviorTreeAsset->BlackboardAsset, BB);

		if (BB)
		{
			BB->ClearValue(TargetBlackBoardKeyName);
			BB->SetValueAsBool("bIsAggroed", false);
		}

		RunBehaviorTree(AIChar->BehaviorTreeAsset);

		if (UBrainComponent* Brain = GetBrainComponent())
		{
			if (!Brain->IsRunning())
			{
				Brain->StartLogic(); 
			}
		}
	}

	// PathFollowing/Perception 재활성화 보장
	if (UPathFollowingComponent* PFC = GetPathFollowingComponent())
	{
		PFC->Activate();
		PFC->SetComponentTickEnabled(true);
	}

	if (UAIPerceptionComponent* PC = GetPerceptionComponent())
	{
		PC->RequestStimuliListenerUpdate();
	}
	
}

void ARedCAIController::OnUnPossess()
{

//	현재 타겟 언바인딩
	 if (UBlackboardComponent* BB = GetBlackboardComponent())
	 {
	 	if (AActor* CurTarget = Cast<AActor>(BB->GetValueAsObject(TargetBlackBoardKeyName)))
	 	{
	 		if (ARedCAICharacter* CurAI = Cast<ARedCAICharacter>(CurTarget))
	 		{
	 			CurAI->OnAIDeath.RemoveDynamic(this, &ARedCAIController::OnTargetAIDeath);
	 		}
	 	}
	 	
	BB->ClearValue(TargetBlackBoardKeyName);
	BB->SetValueAsBool("bIsAggroed", false);
	}
	
	UnbindPerceptionDelegates();
	
	UnbindTowerDestroyedIfNeeded();
	
	// 폰 참조에서 델리게이트 바인딩 플래그 리셋
	if (ARedCAICharacter* AIChar = Cast<ARedCAICharacter>(GetPawn()))
	{
		AIChar->bPerceptionDelegatesBound = false;
	}

	
	if (ARedCAICharacter* AIChar = Cast<ARedCAICharacter>(GetPawn()))
	{
		AIChar->bPerceptionDelegatesBound = false;

		// Stimuli 재등록 타이머 클리어
		GetWorldTimerManager().ClearTimer(AIChar->StimuliTimerHandle);
	}
	if (UAIPerceptionComponent* PC = GetPerceptionComponent())
	{
		PC->ForgetAll(); // 기존 감지 정보 완전 초기화
		PC->RequestStimuliListenerUpdate(); // 감각 시스템 리셋
	}

	UnbindTargetDeathTag();
	
	Super::OnUnPossess();
}

void ARedCAIController::BindPerceptionDelegates()
{
	if (!AIPerceptionComponent) return;
	
	UnbindPerceptionDelegates();

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ARedCAIController::TargetPerceptionUpdate);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ARedCAIController::TargetPerceptionForgotten);
}

void ARedCAIController::UnbindPerceptionDelegates()
{
	if (!AIPerceptionComponent) return;
    
	AIPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &ARedCAIController::TargetPerceptionUpdate);
	AIPerceptionComponent->OnTargetPerceptionForgotten.RemoveDynamic(this, &ARedCAIController::TargetPerceptionForgotten);
}

const UObject* ARedCAIController::GetCurrentTarget() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		return BlackboardComponent->GetValueAsObject(TargetBlackBoardKeyName);
	}

	return nullptr;
}
	
	
void ARedCAIController::TargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor ) return;
	
	if (!Stimulus.WasSuccessfullySensed())
	{
		if (GetCurrentTarget() == Actor)
		{
			ClearAggroState();
		}
		return;
	}
	{
		UAbilitySystemComponent* ASC = nullptr;

		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
		{
			ASC = ASI->GetAbilitySystemComponent();
		}
		else
		{
			ASC = Actor->FindComponentByClass<UAbilitySystemComponent>();
		}

		if (ASC && ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
		{
			// 지금 본 애가 죽어있는 애면 그냥 잊어버리자
			if (UAIPerceptionComponent* PC = GetPerceptionComponent())
			{
				PC->ForgetActor(Actor);
				PC->RequestStimuliListenerUpdate();
			}

			// 내가 그 애를 타겟으로 들고 있던 경우도 초기화
			if (GetCurrentTarget() == Actor)
			{
				ClearAggroState();
			}
			return; // 여기서 끝
		}
	}
	
	// 팀 확인
	ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(*Actor);
	if (Attitude != ETeamAttitude::Hostile) return;

	// 추가 예외 (UI 등)
	if (Actor->ActorHasTag(FName("IgnoreTarget"))) return;  //나중에 필요하면 태그만들어서 무시하기

	const UObject* CurrentTargetObject = GetCurrentTarget();
	AActor* CurrentTarget = const_cast<AActor*>(Cast<AActor>(CurrentTargetObject));

	if (!CurrentTarget)
	{
		SetCurrentTarget(Actor);
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			BB->SetValueAsBool("bIsAggroed", true);
		}
		return;
	}

	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		float CurrentDistance = FVector::Dist(MyPawn->GetActorLocation(), CurrentTarget->GetActorLocation());
		float NewDistance = FVector::Dist(MyPawn->GetActorLocation(), Actor->GetActorLocation());

		if (NewDistance < CurrentDistance)
		{
			SetCurrentTarget(Actor);
			if (UBlackboardComponent* BB = GetBlackboardComponent())
			{
				BB->SetValueAsBool("bIsAggroed", true);
			}
		}
	}
}


AActor* ARedCAIController::GetNextPerceiveActor() const
{
	if (!AIPerceptionComponent)return nullptr;
	
	
	TArray<AActor*> PerceivedActors;
	AIPerceptionComponent->GetCurrentlyPerceivedActors(nullptr,PerceivedActors);

	AActor*BestTarget = nullptr;
	float BestDistance = FLT_MAX;
	APawn* MyPawn = GetPawn();

	if (!MyPawn) return nullptr;

	for (AActor* Actor : PerceivedActors)
	{
		if (!Actor) continue;

		if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Actor))
		{
			if (TeamAgent->GetGenericTeamId() == GetGenericTeamId())
				continue;
		}

		float Distance = FVector::Dist(MyPawn->GetActorLocation(), Actor->GetActorLocation());
		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			BestTarget = Actor;
		}
		
	}	
	return BestTarget;
}

void ARedCAIController::OnTargetTowerDestroyed(AActor* DestroyedTower)
{
	if (!DestroyedTower) return;

	if (const UBlackboardComponent* BB = GetBlackboardComponent())
	{
		if (BB->GetValueAsObject(TargetBlackBoardKeyName) != DestroyedTower)
		{
			return;
		}
	}
	
	ClearAggroState();      
	UnbindTowerDestroyedIfNeeded();
	
	if (UAIPerceptionComponent* PC = GetPerceptionComponent())
	{
		PC->ForgetAll();
		PC->RequestStimuliListenerUpdate();
	}
	
	if (ARedCAICharacter* AIChar = Cast<ARedCAICharacter>(GetPawn()))
	{
		if (AIChar->AbilitySystemComponent)
		{
			AIChar->AbilitySystemComponent->CancelAllAbilities();
		}
		
		if (USkeletalMeshComponent* Mesh = AIChar->GetMesh())
		{
			if (UAnimInstance* Anim = Mesh->GetAnimInstance())
			{
				Anim->StopAllMontages(0.1f);
			}
		}
	}

	//딜레이 넣어서 포탑어그로 다시 안끌리게
	if (UBehaviorTreeComponent* BTC = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		FTimerHandle RestartHandle;
		GetWorld()->GetTimerManager().SetTimer(
			RestartHandle,
			[this, BTC]()
			{
				if (UBlackboardComponent* BB = GetBlackboardComponent())
				{
					BB->SetValueAsBool("bIsAggroed", false);
				}
				BTC->StopTree(EBTStopMode::Safe);
				BTC->RestartTree();
			},
			0.1f,
			false
		);
	}
	
	AActor* NextTarget = GetNextPerceiveActor();
	if (NextTarget)
	{
		SetCurrentTarget(NextTarget);
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			BB->SetValueAsBool("bIsAggroed", true);
		}
	}
}

void ARedCAIController::BindTowerDestroyedIfNeeded(AActor* NewTarget)
{
    	
	if (ARedCAI_Tower* Tower = Cast<ARedCAI_Tower>(NewTarget))
	{
		if (!Tower -> OnTowerDestroyed.IsAlreadyBound(this, &ARedCAIController::OnTargetTowerDestroyed))
		{
			Tower->OnTowerDestroyed.AddDynamic(this, &ARedCAIController::OnTargetTowerDestroyed);
		}
		BoundTower = Tower;
	}
    	
}

void ARedCAIController::UnbindTowerDestroyedIfNeeded()
{
	if (BoundTower.IsValid())
	{
		if (BoundTower->OnTowerDestroyed.IsAlreadyBound(this, &ARedCAIController::OnTargetTowerDestroyed))
		{
			BoundTower->OnTowerDestroyed.RemoveDynamic(this, &ARedCAIController::OnTargetTowerDestroyed);
		}
	}
	BoundTower.Reset();
}

void ARedCAIController::BindTargetDeathTag(AActor* NewTarget)
{
	if (!NewTarget)
		return;

	UAbilitySystemComponent* ASC = nullptr;

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(NewTarget))
	{
		ASC = ASI->GetAbilitySystemComponent();
	}
	else
	{
		ASC = NewTarget->FindComponentByClass<UAbilitySystemComponent>();
	}

	if (!ASC)
		return;

	// 기존 연결 제거
	UnbindTargetDeathTag();

	TargetASC = ASC;

	TargetDeathTagHandle =
		ASC->RegisterGameplayTagEvent(PlayerStateTags::Player_State_Death, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &ARedCAIController::OnTargetDeathTagChanged);
}

void ARedCAIController::UnbindTargetDeathTag()
{
	if (TargetASC.IsValid() && TargetDeathTagHandle.IsValid())
	{
		TargetASC->RegisterGameplayTagEvent(PlayerStateTags::Player_State_Death, EGameplayTagEventType::NewOrRemoved)
			.Remove(TargetDeathTagHandle);
	}
	TargetASC = nullptr;
	TargetDeathTagHandle = FDelegateHandle();
}

void ARedCAIController::OnTargetDeathTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag != PlayerStateTags::Player_State_Death) return;

	AActor* CurrentTarget = const_cast<AActor*>(Cast<AActor>(GetCurrentTarget()));

	// 사망 처리
	if (NewCount > 0)
	{
		if (!CurrentTarget) return;

		UE_LOG(LogTemp, Warning, TEXT("[%s] DeathTag detected on %s"), *GetNameSafe(this), *GetNameSafe(CurrentTarget));

		ClearAggroState();

		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			BB->SetValueAsBool("bIsAggroed", false);
			BB->ClearValue(TargetBlackBoardKeyName);
		}

		if (UAIPerceptionComponent* PC = GetPerceptionComponent())
		{
			PC->ForgetActor(CurrentTarget);
			PC->RequestStimuliListenerUpdate();
		}

		if (ARedCAICharacter* AIChar = Cast<ARedCAICharacter>(GetPawn()))
		{
			if (AIChar->AbilitySystemComponent)
				AIChar->AbilitySystemComponent->CancelAllAbilities();

			if (USkeletalMeshComponent* Mesh = AIChar->GetMesh())
			{
				if (UAnimInstance* Anim = Mesh->GetAnimInstance())
					Anim->StopAllMontages(0.1f);
			}
		}

		if (ARedCAICharacter* DeadAI = Cast<ARedCAICharacter>(CurrentTarget))
		{
			if (DeadAI->OnAIDeath.IsAlreadyBound(this, &ARedCAIController::OnTargetAIDeath))
				DeadAI->OnAIDeath.RemoveDynamic(this, &ARedCAIController::OnTargetAIDeath);
		}

		return; // 리스폰 시 분기 안 타게 리턴
	}

	//  리스폰(DeathTag 제거 시) 처리
	if (NewCount == 0)
	{
		if (!TargetASC.IsValid()) return;

		AActor* RespawnedActor = TargetASC->GetOwner();
		if (!RespawnedActor) return;

		UE_LOG(LogTemp, Warning, TEXT("[%s] %s respawned → re-register perception"),
			*GetNameSafe(this), *GetNameSafe(RespawnedActor));

		// 감각 시스템 다시 갱신
		if (UAIPerceptionComponent* PC = GetPerceptionComponent())
		{
			PC->RequestStimuliListenerUpdate();
		}

		// 어그로 초기화 (필요 시)
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			BB->ClearValue(TargetBlackBoardKeyName);
			BB->SetValueAsBool("bIsAggroed", false);
		}
	}
}


void ARedCAIController::ClearAggroState()
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		if (AActor* CurTarget = Cast<AActor>(BB->GetValueAsObject(TargetBlackBoardKeyName)))
		{
			if (ARedCAICharacter* CurAI = Cast<ARedCAICharacter>(CurTarget))
			{
				CurAI->OnAIDeath.RemoveDynamic(this, &ARedCAIController::OnTargetAIDeath);
			}
		}

		BB->ClearValue(TargetBlackBoardKeyName);
		BB->SetValueAsBool("bIsAggroed", false);
		//UE_LOG(LogTemp, Warning, TEXT("[%s] Target cleared, aggro reset."), *GetNameSafe(this));
	}
}

void ARedCAIController::OnTargetAIDeath(ARedCAICharacter* DeadAI)
{
	// DeadAI가 현재 타겟이면 어그로 정리
	if (DeadAI && DeadAI == Cast<AActor>(GetCurrentTarget()))
	{
		// 더 이상 이벤트 안받도록 언바인딩 (안전)
		DeadAI->OnAIDeath.RemoveDynamic(this, &ARedCAIController::OnTargetAIDeath);
		ClearAggroState();
	}
}

void ARedCAIController::TargetPerceptionForgotten(AActor* Actor)
{
	if (!Actor) return;
	
	//다음 타겟을 설정가능
	if (GetCurrentTarget() == Actor)
	{
		AActor* NextTarget = GetNextPerceiveActor();
		SetCurrentTarget(NextTarget);
        
		// 새 타겟이 없으면 aggro 해제
		if (!NextTarget)
		{
			if (UBlackboardComponent* BB = GetBlackboardComponent())
			{
				BB->SetValueAsBool("bIsAggroed", false);
			}
		}
	}
}

void ARedCAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		return;
	}
	
	// 이전 타겟의 dead 델리게이트 해제
	if (AActor* OldTarget = Cast<AActor>(BB->GetValueAsObject(TargetBlackBoardKeyName)))
	{
		if (ARedCAICharacter* OldAI = Cast<ARedCAICharacter>(OldTarget))
		{
			if (OldAI->OnAIDeath.IsAlreadyBound(this, &ARedCAIController::OnTargetAIDeath))
			{
				OldAI->OnAIDeath.RemoveDynamic(this, &ARedCAIController::OnTargetAIDeath);
			}
		}
	}
	
	UnbindTargetDeathTag();
	UnbindTowerDestroyedIfNeeded();
	
	if (NewTarget)
	{
		BB->SetValueAsObject(TargetBlackBoardKeyName, NewTarget);

		
		
		// 새 타겟이 AI라면 사망 델리게이트 바인딩
		if (ARedCAICharacter* NewAI = Cast<ARedCAICharacter>(NewTarget))
		{
			if (!NewAI->OnAIDeath.IsAlreadyBound(this, &ARedCAIController::OnTargetAIDeath))
			{
				NewAI->OnAIDeath.AddDynamic(this, &ARedCAIController::OnTargetAIDeath);
			}
			//UE_LOG(LogTemp, Warning, TEXT("[%s] Bound OnAIDeath to %s"), *GetNameSafe(this), *GetNameSafe(NewAI));
		}
		BindTowerDestroyedIfNeeded(NewTarget);
		BindTargetDeathTag(NewTarget);
	}
	else
	{
		BB->ClearValue(TargetBlackBoardKeyName);
	}
}

FGenericTeamId ARedCAIController::GetTeamIdFromActor(const AActor* Actor) const
{
	if (!Actor) return FGenericTeamId::NoTeam;

	
	//  ICharacterIdentifier 인터페이스로부터 직접 읽기 (ARedCCharacter 계열)
	if (const ICharacterIdentifier* CharId = Cast<ICharacterIdentifier>(Actor))
	{
		return FGenericTeamId(CharId->GetCharacterTeamId_UINT8());
	}

	// IGenericTeamAgentInterface 구현된 경우
	if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Actor))
	{
		return TeamAgent->GetGenericTeamId();
	}

	// Pawn → Controller → PlayerState 순서로 팀 가져오기
	if (const APawn* TestPawn = Cast<APawn>(Actor))
	{
		if (const AController* C = TestPawn->GetController())
		{
			if (const IGenericTeamAgentInterface* CtrlItf = Cast<IGenericTeamAgentInterface>(C))
				return CtrlItf->GetGenericTeamId();
		}
		if (const APlayerState* PS = TestPawn->GetPlayerState())
		{
			if (const IGenericTeamAgentInterface* PSI = Cast<IGenericTeamAgentInterface>(PS))
				return PSI->GetGenericTeamId();
		}
	}

	// Owner 체인
	if (const AActor* TestOwner = Actor->GetOwner())
	{
		if (const IGenericTeamAgentInterface* OI = Cast<IGenericTeamAgentInterface>(TestOwner))
			return OI->GetGenericTeamId();
	}
	
	return FGenericTeamId::NoTeam;
}

ETeamAttitude::Type ARedCAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	
	FGenericTeamId MyTeam = GetGenericTeamId();
	FGenericTeamId OtherTeam = GetTeamIdFromActor(&Other);

	if (OtherTeam == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Neutral;
	}
	return (OtherTeam == MyTeam) ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
}
