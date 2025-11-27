// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter/RedCAI_Tower.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "Characters/RedCPlayerCharacter.h"
#include "TimerManager.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/RedCGameplayTags.h" 
#include "AbilitySystemGlobals.h"
#include "GenericTeamAgentInterface.h"   
#include "GameplayEffect.h"
#include "Perception/AISense_Sight.h"
#include "AbilitySystemComponent.h" 
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Types/RedCLogChannels.h"
#include "NiagaraFunctionLibrary.h" 
#include "Characters/RedCHeroCharacter.h"
#include "Frameworks/GameState/GameStateArena.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "UI/Widget/Battle/OverheadHealthWidget.h"


ARedCAI_Tower::ARedCAI_Tower()
{
	PrimaryActorTick.bCanEverTick = false;

	SourceASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC_Tower"));
	SourceASC->SetIsReplicated(true);
	SourceASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	HealthSet = CreateDefaultSubobject<URedCHealthSet>(TEXT("HealthSet"));
	SourceASC->AddAttributeSetSubobject(HealthSet);
	
	// 루트를 타워 메시로 지정
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = StaticMeshComponent;
	
	SphereComponent =  CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent -> SetupAttachment(RootComponent);
	SphereComponent -> SetSphereRadius(500.f);
	SphereComponent -> SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
	SphereComponent -> OnComponentBeginOverlap.AddDynamic(this, &ARedCAI_Tower::OnBeginOverlap);
	SphereComponent -> OnComponentEndOverlap.AddDynamic(this, &ARedCAI_Tower::OnEndOverlap);
    
	bHasTarget = false;
	CurrentTarget = nullptr;

	bReplicates = true;
	
	AOEObjectTypes.Reset();
	AOEObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	AOEObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	if (StimuliSource)
	{
		
		StimuliSource->RegisterWithPerceptionSystem();
	}

	OverheadHealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadHealthWidgetComponent"));
	OverheadHealthWidgetComp->SetHiddenInGame(true);
	OverheadHealthWidgetComp->SetupAttachment(GetRootComponent());
}

void ARedCAI_Tower::BeginPlay()
{
	Super::BeginPlay();

	if (StimuliSource)
	{
		StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
		StimuliSource->RegisterWithPerceptionSystem();
		GetWorldTimerManager().SetTimerForNextTick([this]()
			   {
				   if (StimuliSource)
				   {
					   StimuliSource->RegisterWithPerceptionSystem();
				   }
			   });
		
	}
	// ASC 초기화(Owner/Avatar 모두 자신으로)
	if (SourceASC)
	{
		SourceASC->InitAbilityActorInfo(this, this);
	}
	
	//초기 스탯 적용
	if (HasAuthority()&& HealthSet)
	{
		if (GE_TowerInitialize)
		{
			FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
			Ctx.AddSourceObject(this);
			
			FGameplayEffectSpecHandle SpecH = SourceASC->MakeOutgoingSpec(GE_TowerInitialize, 1.f, Ctx);

			if (SpecH.IsValid() && SpecH.Data.IsValid())
			{
				SpecH.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_MaxHealth, TowerData.MaxHealth);
				SpecH.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_Health, TowerData.Health);
				
				SourceASC->ApplyGameplayEffectSpecToSelf(*SpecH.Data.Get());
			}
		}
		
		if (SourceASC)
		{
			// Health 변화 델리게이트
			SourceASC->GetGameplayAttributeValueChangeDelegate(HealthSet->GetHealthAttribute()).AddUObject(this, &ARedCAI_Tower::HandleHealthChanged);
		}
	}
	MinionQueue.Empty();
	ChampionQueue.Empty();

	if (HasAuthority())
	{
		// 서버에 GS에 포탑 등록
		RegisterTowerToGS();
	}

	ConfigureOverheadHealthWidget();
	SetTeamColorOverlayMaterial();
}


void ARedCAI_Tower::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	if (!OtherActor || OtherActor == this) return;

	if (!IsHostileTo(OtherActor))
	{
		return;
	}
	if (ARedCAICharacter* Minion = Cast<ARedCAICharacter>(OtherActor))
	{
		if (!MinionQueue.Contains(Minion))
		{
			MinionQueue.Add(Minion);
		}
	}
	else if (ARedCPlayerCharacter* Champion = Cast<ARedCPlayerCharacter>(OtherActor))
	{
		if (!ChampionQueue.Contains(Champion))
		{
			ChampionQueue.Add(Champion);
		}
	}

	if (!bHasTarget)
	UpdateTarget();
}

void ARedCAI_Tower::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (!HasAuthority()) return;
	
	if (!OtherActor) return;
	
	if (MinionQueue.Contains(OtherActor))
 	{
 		MinionQueue.Remove(OtherActor);
 	}
 	else if (ChampionQueue.Contains(OtherActor))
 	{
 		ChampionQueue.Remove(OtherActor);
 	}
 
 	if (OtherActor == CurrentTarget)
 	{
 		CurrentTarget = nullptr;
 		bHasTarget = false;
 		UpdateTarget();
 	}
}

void ARedCAI_Tower::OnRep_TeamId()
{
	//UE_LOG(LogRedC, Warning, TEXT("[Tower] OnRep_TeamId -> %d"), TeamId);
}


void ARedCAI_Tower::OnEnemyDied(AActor* DeadActor)
{
	if (!DeadActor) return;
	
	// 큐 정리
	MinionQueue.Remove(DeadActor);
	ChampionQueue.Remove(DeadActor);
	
 	if(DeadActor == CurrentTarget)
 	{
 		CurrentTarget = nullptr;
 		UpdateTarget();
 	}
}


void ARedCAI_Tower::SetGenericTeamId(const FGenericTeamId& InTeamId)
{
	TeamId = InTeamId;
}

FGameplayTag ARedCAI_Tower::GetCharacterTag() const
{
	return CharacterTags::Character_Tower.GetTag();
}

uint8 ARedCAI_Tower::GetCharacterTeamId_UINT8() const
{
	return TeamId.GetId();
}

bool ARedCAI_Tower::IsCharacterSameTeam(const ICharacterIdentifier* Other) const
{
	if (TeamId.GetId() == Other->GetCharacterTeamId_UINT8())
	{
		return true;
	}
	return false;
}

void ARedCAI_Tower::SetTeamId(uint8 NewId)
{
	SetGenericTeamId(FGenericTeamId(NewId));
}

void ARedCAI_Tower::TowerDestroy()
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (bIsBeingDestroyed)
	{
		return;
	}
	bIsBeingDestroyed = true;

	UnbindTargetDeathTag(); 
	StopAttacking();
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	OnTowerDestroyed.Broadcast(this);
	UE_LOG(LogRedC, Warning, TEXT("[Tower] OnTowerDestroyed.Broadcast done | Name=%s , TowerDestroy 함수"), *GetName());

	FTimerHandle SafeDestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(SafeDestroyHandle, [this]()
	{
		if (IsValid(this))
		{
			Destroy();
		}
	}, 0.05f, false);
	
}

void ARedCAI_Tower::UpdateTarget()
{
	MinionQueue.RemoveAll([this](AActor* A){ return !IsValid(A) || !IsHostileTo(A); }); 
	ChampionQueue.RemoveAll([this](AActor* A){ return !IsValid(A) || !IsHostileTo(A); });

	if (MinionQueue.Num() > 0)
	{
		CurrentTarget = MinionQueue[0];
		bHasTarget = true;
	}
	else if (ChampionQueue.Num() > 0)
	{
		CurrentTarget = ChampionQueue[0];
		bHasTarget = true;
	}
	else
	{
		bHasTarget = false;
		CurrentTarget = nullptr;
	}

	if (bHasTarget)
	{
		BindTargetDeathTag(CurrentTarget);
		StartAttacking();
	}
	else
	{
		UnbindTargetDeathTag(); 
		StopAttacking();
	}
}

void ARedCAI_Tower::StartAttacking()
{
	if (!HasAuthority())
	{
		return;
	}

	AttackTime = FMath::Max(AttackTime, PreAttackDelay + 0.05f);

	GetWorld()->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ARedCAI_Tower::AttackTarget,
		AttackTime,
		true
	);

}

void ARedCAI_Tower::StopAttacking()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
}

void ARedCAI_Tower::AttackTarget()
{
	if (!HasAuthority())
	{
		return;
	}

	if (CurrentTarget && bHasTarget)
	{
		const FVector TargetLocation = CurrentTarget->GetActorLocation();

		//공격 범위 표시
		Multicast_ShowAttackIndicator(TargetLocation);

		//에너지 모으는 이펙트
		Multicast_PlayChargeFX();

		// 데미지 처리 예약
		FTimerHandle ExplosionTimerHandle;
		FTimerDelegate Del;
		Del.BindUObject(this,&ARedCAI_Tower::PerformRadialDamage_GAS, TargetLocation);
		GetWorld()->GetTimerManager().SetTimer(
			ExplosionTimerHandle,
			Del,
			PreAttackDelay,
			false
		);
	}
	else
	{
		// 타겟 없으면 멈춤
		StopAttacking();
	}
}

void ARedCAI_Tower::ShowAttackIndicator(const FVector& Location)
{
	if (!IndicatorMesh || !GetWorld() || GetWorld()->WorldType == EWorldType::Editor)
	{
		return;
	}

	UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(
	   this, UStaticMeshComponent::StaticClass(), NAME_None, RF_Transient
   );
	if (!Comp)
	{
		return;
	}

	Comp->SetStaticMesh(IndicatorMesh);
	if (IndicatorMaterial) Comp->SetMaterial(0, IndicatorMaterial);

	Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Comp->SetMobility(EComponentMobility::Movable);
	Comp->SetGenerateOverlapEvents(false);
	Comp->SetCastShadow(false);
	Comp->SetCanEverAffectNavigation(false);
	Comp->RegisterComponent();
	Comp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	const FVector SpawnLoc = Location + FVector(0,0, IndicatorZOffset_Mesh - 50.f);
	Comp->SetWorldLocation(SpawnLoc);
	Comp->SetWorldRotation(FRotator::ZeroRotator);
	
	Comp->SetWorldScale3D(FVector(IndicatorScale));

	// PreAttackDelay 뒤 자동 제거
	TWeakObjectPtr<UStaticMeshComponent> WeakComp = Comp;
	FTimerHandle H;
	GetWorld()->GetTimerManager().SetTimer(H, [WeakComp]()
	{
		if (WeakComp.IsValid())
		{
			WeakComp->UnregisterComponent();
			WeakComp->DestroyComponent();
		}
	}, PreAttackDelay, false);

}

void ARedCAI_Tower::Multicast_ShowAttackIndicator_Implementation(FVector Location)
{
	ShowAttackIndicator(Location);
}

void ARedCAI_Tower::PerformRadialDamage_GAS(FVector Location)
{
	if (!HasAuthority()) return;

	Multicast_PlayChargeReleaseFX(); //포탑입구
	
	Multicast_PlayFX(Location); // 터지는 위치
	
	UAbilitySystemComponent* SourceASC_Tower = SourceASC;
	if (bUseOwnerASCAsSource && GetOwner())
	{
		if (UAbilitySystemComponent* OwnerASC =	GetOwner()->FindComponentByClass<UAbilitySystemComponent>())
		{
			SourceASC_Tower = OwnerASC;
		}
	}

	if (!SourceASC_Tower || !DamageGameplayEffectClass)
	{
		return;
	}

	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		this,
		Location,
		DamageRadius,
		AOEObjectTypes,
		AActor::StaticClass(),
		ActorsToIgnore,
		OutActors
	);

	if (!bHit || OutActors.Num() == 0)
	{
		return;
	}
	
	if (!SourceASC_Tower || !DamageGameplayEffectClass)
	{
		return;
	}
	
	// Context에 소스 오브젝트 추가(추후 GameplayCue/Instigator 추적에 유용)
	FGameplayEffectContextHandle Ctx = SourceASC_Tower->MakeEffectContext();
	Ctx.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = SourceASC_Tower->MakeOutgoingSpec(
		DamageGameplayEffectClass, 1.f, Ctx
	);

	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}
	
	SpecHandle.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_Health, -DamageAmount);
	
	const EGameplayEffectDurationType DurationType = SpecHandle.Data->Def.Get()->DurationPolicy;
	
	// 4) 각 타겟 ASC에 적용
	int32 Applied = 0;
	for (AActor* Victim : OutActors)
	{
		if (!IsValid(Victim) || Victim == this)
		{
			continue;
		}
		
		const FGenericTeamId MyTeam = GetGenericTeamId();
		const FGenericTeamId OtherTeam = ResolveTeamIdFromActor(Victim);
		
		if (MyTeam == OtherTeam)
		{
			continue; //  같은 팀이면 스킵
		}

		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Victim, true);

		if (!TargetASC)
		{
			continue;
		}
		if (TargetASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Invincible) || TargetASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
		{
			continue;
		}
		
		const FActiveGameplayEffectHandle Handle = SourceASC_Tower->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

		const bool bAppliedOk = (DurationType == EGameplayEffectDurationType::Instant) || Handle.IsValid();
    	
		if (bAppliedOk)
		{
			++Applied;
		}
	}
	
}

void ARedCAI_Tower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARedCAI_Tower, TeamId); 
}

void ARedCAI_Tower::Multicast_PlayChargeReleaseFX_Implementation()
{
	if (IsNetMode(NM_DedicatedServer)) return;

	if (NS_ChargeRelease && StaticMeshComponent)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			NS_ChargeRelease,
			StaticMeshComponent,
			FName("Burst"),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true,  // bAutoDestroy
			true,  // bAutoActivate
			ENCPoolMethod::AutoRelease
		);
	}
}
void ARedCAI_Tower::Multicast_PlayChargeFX_Implementation()
{
	if (IsNetMode(NM_DedicatedServer)) return;

	if ( VFX_Map.FindRef(FName("Charge")) && StaticMeshComponent)
	{
		// 소켓 없으면 Root 기준
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			VFX_Map.FindRef(FName("Charge")),
			StaticMeshComponent,
			FName("Charge"), 
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true,  // bAutoDestroy
			true   // bAutoActivate
		);
	}
}


// 체력 변경 처리
void ARedCAI_Tower::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!HasAuthority()) return;
	
	const float OldHP = Data.OldValue;
	const float NewHP = Data.NewValue;
	
	UE_LOG(LogRedC, Warning, TEXT("[Tower] HealthChanged: %.1f → %.1f"), OldHP, NewHP);
	
	if (NewHP <= 0.f)
	{
		TowerDestroy();
	}
}



FGenericTeamId ARedCAI_Tower::ResolveTeamIdFromActor(const AActor* Actor)
{
	if (!Actor) return FGenericTeamId::NoTeam;

	// 1. ICharacterIdentifier 지원 (플레이어용)
	if (const ICharacterIdentifier* CharId = Cast<ICharacterIdentifier>(Actor))
	{
		return FGenericTeamId(CharId->GetCharacterTeamId_UINT8());
	}

	// 2. 기존 GenericTeamAgent 체크
	if (const IGenericTeamAgentInterface* Itf = Cast<IGenericTeamAgentInterface>(Actor))
	{
		return Itf->GetGenericTeamId();
	}

	// 3. Pawn → Controller / PlayerState 순서
	if (const APawn* TestPawn = Cast<APawn>(Actor))
	{
		if (const AController* C = TestPawn->GetController())
		{
			if (const IGenericTeamAgentInterface* CItf = Cast<IGenericTeamAgentInterface>(C))
				return CItf->GetGenericTeamId();
		}
		if (const APlayerState* PS = TestPawn->GetPlayerState())
		{
			if (const IGenericTeamAgentInterface* PSItf = Cast<IGenericTeamAgentInterface>(PS))
				return PSItf->GetGenericTeamId();
		}
	}

	// 4) Owner 체인
	if (const AActor* TestOwner = Actor->GetOwner())
	{
		if (const IGenericTeamAgentInterface* OItf = Cast<IGenericTeamAgentInterface>(TestOwner))
			return OItf->GetGenericTeamId();

		if (const ICharacterIdentifier* OCharId = Cast<ICharacterIdentifier>(TestOwner))
			return FGenericTeamId(OCharId->GetCharacterTeamId_UINT8());
	}

	return FGenericTeamId::NoTeam;
}

bool ARedCAI_Tower::IsHostileTo(const AActor* Other) const
{
	if (!Other) return false;

	const FGenericTeamId MyTeam = GetGenericTeamId();
	const FGenericTeamId OtherTeam = ResolveTeamIdFromActor(Other);

	// 로그로 확인해보면 좋음
	// UE_LOG(LogRedC, Warning, TEXT("[Tower][TeamCheck] %s(%d) vs %s(%d)"),
	//	*GetName(), MyTeam.GetId(), *Other->GetName(), OtherTeam.GetId());

	if (OtherTeam == FGenericTeamId::NoTeam)
	{
		// 팀 없는 대상은 중립으로 취급
		return false;
	}

	// 같은 팀이면 적대 아님
	return MyTeam != OtherTeam;
}

void ARedCAI_Tower::Multicast_PlayFX_Implementation(const FVector& Location)
{
	if (IsNetMode(NM_DedicatedServer)) return;

	if (auto Impact = VFX_Map.FindRef(FName("Impact")))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			Impact,
			Location,
	FRotator::ZeroRotator,
	   FVector(1.f),
			true,  // bAutoDestroy
			true,  // bAutoActivate
			ENCPoolMethod::AutoRelease
		);
	}
	if (USoundCue* Sound = SFX_Map.FindRef(FName("Explosion")))
	{
		UGameplayStatics::SpawnSoundAtLocation(this, Sound, Location);
		//UGameplayStatics::PlaySoundAtLocation();
	}
	if (USoundCue* Sound = SFX_Map.FindRef(FName("Fire")))
	{
		UGameplayStatics::SpawnSoundAtLocation(this, Sound, GetActorLocation());
	}
}

void ARedCAI_Tower::BindTargetDeathTag(AActor* NewTarget)
{
    // 먼저 기존 거 끊고
    UnbindTargetDeathTag();

    if (!NewTarget)
        return;

    UAbilitySystemComponent* ASC = nullptr;

    // Actor가 ASC를 바로 들고 있는 경우
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

    CurrentTargetASC = ASC;

    TargetDeathTagHandle =
        ASC->RegisterGameplayTagEvent(PlayerStateTags::Player_State_Death, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &ARedCAI_Tower::OnTargetDeathTagChanged);
}

void ARedCAI_Tower::UnbindTargetDeathTag()
{
    if (CurrentTargetASC.IsValid() && TargetDeathTagHandle.IsValid())
    {
        CurrentTargetASC->RegisterGameplayTagEvent(PlayerStateTags::Player_State_Death, EGameplayTagEventType::NewOrRemoved)
            .Remove(TargetDeathTagHandle);
    }

    CurrentTargetASC = nullptr;
    TargetDeathTagHandle = FDelegateHandle();
}


void ARedCAI_Tower::OnTargetDeathTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag != PlayerStateTags::Player_State_Death)
		return;

	// 죽음 태그 붙음
	if (NewCount > 0)
	{
		AActor* DeadTarget = CurrentTarget;
		// 큐에서도 제거
		if (DeadTarget)
		{
			MinionQueue.Remove(DeadTarget);
			ChampionQueue.Remove(DeadTarget);
		}

		CurrentTarget = nullptr;
		bHasTarget = false;

		UnbindTargetDeathTag();
		StopAttacking();

		// 남아 있는 애로 다시 선택
		UpdateTarget();
	}
}


void ARedCAI_Tower::RegisterTowerToGS()
{
	if (AGameStateArena* GS = GetWorld()->GetGameState<AGameStateArena>())
    	{
    		if (TeamId.GetId() == 1) //Red Team
    		{
    			GS->AddRedTeamTowerArray(this);
    		}
    		else if (TeamId.GetId() == 0) //Blue Team
    		{
    			GS->AddBlueTeamTowerArray(this);
    		}
    	}
}

void ARedCAI_Tower::SetTeamColorOverlayMaterial()
{
	UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(GetRootComponent());
	if (!MeshComponent) return;
	
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (const ARedCHeroCharacter* Character = PC->GetPawn<ARedCHeroCharacter>())
		{
			MeshComponent->SetOverlayMaterial(TeamColorOverlayMaterial[IsCharacterSameTeam(Character)]);
		}
	}
}

void ARedCAI_Tower::ConfigureOverheadHealthWidget()
{
	check(OverheadHealthWidgetComp);

	OverheadHealthWidgetComp->SetHiddenInGame(false);

	UOverheadHealthWidget* OverheadHealthWidget = Cast<UOverheadHealthWidget>(OverheadHealthWidgetComp->GetUserWidgetObject());
	if (!OverheadHealthWidget) return;

	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			OverheadHealthWidget->BindHealthAttribute(ASC);
			
			GetWorldTimerManager().ClearTimer(OverheadHealthWidgetUpdateTimerHandle);
			GetWorldTimerManager().SetTimer(OverheadHealthWidgetUpdateTimerHandle,
				this, &ThisClass::UpdateOverheadHealthWidgetVisibility, OverheadHealthWidgetCheckInterval, true);
		}

		if (const ARedCHeroCharacter* Character = PC->GetPawn<ARedCHeroCharacter>())
		{
			OverheadHealthWidget->SetTeamColor(IsCharacterSameTeam(Character));
		}
	}
}

void ARedCAI_Tower::UpdateOverheadHealthWidgetVisibility()
{
	OverheadHealthWidgetComp->SetHiddenInGame(!(IsOverheadHealthWidgetInRange() && IsOverheadHealthWidgetOnScreen()));
}

bool ARedCAI_Tower::IsOverheadHealthWidgetOnScreen() const
{
	const APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	
	if (!PC)
	{
		return false;
	}

	const FVector WidgetWorldLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
	FVector2D ScreenPosition;
	int32 ViewportX, ViewportY;

	PC->GetViewportSize(ViewportX, ViewportY);

	const bool bIsOnScreen = PC->ProjectWorldLocationToScreen(WidgetWorldLocation, ScreenPosition);
	const bool bIsViewportBounds = (ScreenPosition.X >= 0.f) && (ScreenPosition.Y <= ViewportX) && (ScreenPosition.Y >= 0.f) && (ScreenPosition.Y <= ViewportY);

	return bIsOnScreen && bIsViewportBounds;
}

bool ARedCAI_Tower::IsOverheadHealthWidgetInRange() const
{
	if (const APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		const float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());

		return DistSquared <= OverheadHealthWidgetVisibleRange;
	}

	return false;
}
