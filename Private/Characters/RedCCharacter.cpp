// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RedCCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "ContextualAnimSceneActorComponent.h"
#include "ContextualAnimSceneAsset.h"
#include "ContextualAnimUtilities.h"
#include "AI/AICharacter/RedCAI_Tower.h"
#include "Characters/DefaultMontageData.h"
#include "Characters/Components/CombatComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Controllers/RedCPlayerController.h"
#include "Core/Systems/RedCAssetManager.h"
#include "Frameworks/GameMode/GameModeArena.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Abilities/RedCItemBuyAbility.h"
#include "GAS/Effects/InvincibleEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/RedCFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCAbilityTypes.h"
#include "Types/RedCLogChannels.h"
#include "UI/Widget/Battle/OverheadHealthWidget.h"


ARedCCharacter::ARedCCharacter()
	: IsDead(false)
{
	// Mesh Decal 투영 비활성화
	GetMesh()->bReceivesDecals = false;

	// Mesh Collision 비활성화
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ContextualAnimSceneActorComp = CreateDefaultSubobject<UContextualAnimSceneActorComponent>(TEXT("ContextualAnimSceneActorComponent"));
	ContextualAnimSceneActorComp->SetIsReplicated(true);
	ContextualAnimSceneActorComp->SetupAttachment(RootComponent);

	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	CombatSystem = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatSystem"));

	StandingHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StandingHitBox"));
	StandingHitBox->SetupAttachment(GetMesh());
	StandingHitBox->SetCollisionProfileName(TEXT("HitBoxProfile"));
	StandingHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	KnockdownHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("KnockdownHitBox"));
	KnockdownHitBox->SetupAttachment(GetMesh());
	KnockdownHitBox->SetCollisionProfileName(TEXT("HitBoxProfile"));
	KnockdownHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverheadHealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadHealthWidgetComponent"));
	OverheadHealthWidgetComp->SetHiddenInGame(true);
	OverheadHealthWidgetComp->SetupAttachment(GetRootComponent());
}

void ARedCCharacter::InitAbilitySystemComponent()
{
	// 자식에서 구현 (Super 함수 호출 X)
}

void ARedCCharacter::BindCallbacksToDependencies()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		ASC->RegisterGameplayTagEvent(PlayerStateTags::Player_State_Death).AddUObject(this, &ARedCCharacter::DeathTagUpdated);
		//ASC->RegisterGameplayTagEvent(PlayerStateTags::Player_State_Invincible).AddUObject(this, &ARedCCharacter::EnterInvincibleState);
	}
}

bool ARedCCharacter::IsCharacterHasTag(const FGameplayTag& Tag) const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			return true;
		}
	}

	return false;
}

UAbilitySystemComponent* ARedCCharacter::GetAbilitySystemComponent() const
{
	return nullptr; // 자식에서 구현 (Super 함수 호출 X)
}

void ARedCCharacter::ResolveCharacterDefinition()
{
	const APlayerStateArena* PS = GetPlayerState<APlayerStateArena>();
	if (!PS)
	{
		return;
	}
	
	CharacterTag = FGameplayTag::RequestGameplayTag(FName(PS->GetPlayerTravelInfo().CharacterID));

	if (!CharacterTag.IsValid())
	{
		return;
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
		if (not GetUpMontage)
		{
			UE_LOG(LogRedC, Warning, TEXT("Get Up Montage is not found."));
			return;
		}
		DeathMontage = MontageData->MontageMap.Find(PlayerStateTags::Player_State_Death)->Get();
		if (not DeathMontage)
		{
			UE_LOG(LogRedC, Warning, TEXT("Death Montage is not found."));
			return;
		}
		RespawnMontage = MontageData->MontageMap.Find(PlayerStateTags::Player_State_Respawn)->Get();
		if (not RespawnMontage)
		{
			UE_LOG(LogRedC, Warning, TEXT("Respawn Montage is not found."));
			return;
		}
	}
	else
	{
		UE_LOG(LogRedC, Warning, TEXT("Montage Data is not found."));
		return;
	}
}

bool ARedCCharacter::IsOverheadHealthWidgetOnScreen() const
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

	// 위젯이 화면 범위 내에 있는지 확인
	const bool bIsOnScreen = PC->ProjectWorldLocationToScreen(WidgetWorldLocation, ScreenPosition);
	const bool bIsViewportBounds = (ScreenPosition.X >= 0.f) && (ScreenPosition.Y <= ViewportX) && (ScreenPosition.Y >= 0.f) && (ScreenPosition.Y <= ViewportY);

	return bIsOnScreen && bIsViewportBounds;
}

bool ARedCCharacter::IsOverheadHealthWidgetInRange() const
{
	if (const APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		const float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());

		// 위젯이 일정 거리 내에 있는지 확인
		return DistSquared <= OverheadHealthWidgetVisibleRange;
	}

	return false;
}

void ARedCCharacter::ConfigureOverheadHealthWidget()
{
	check(OverheadHealthWidgetComp);

	if (GetController() && GetController()->IsLocalPlayerController())
	{
		OverheadHealthWidgetComp->SetHiddenInGame(true);
		return;
	}

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

		if (const ARedCCharacter* Character = PC->GetPawn<ARedCCharacter>())
		{
			OverheadHealthWidget->SetTeamColor(IsCharacterSameTeam(Character));
		}
	}
}

void ARedCCharacter::UpdateOverheadHealthWidgetVisibility()
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	
	if (!IsValid(ASC))
	{
		return;
	}

	const bool bIsEncounter = ASC->HasMatchingGameplayTag(PlayerStateTags::Player_Mode_InCombat);
	const bool bIsDead = ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death);

	OverheadHealthWidgetComp->SetHiddenInGame(bIsDead || !bIsEncounter || !(IsOverheadHealthWidgetInRange() && IsOverheadHealthWidgetOnScreen()));
}

#pragma region HitBox State
void ARedCCharacter::SwitchHitBoxStateToKnockdown_Implementation()
{
	//UE_LOG(LogRedC, Log, TEXT("Switch Collision KnockedDown"));
	StandingHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	KnockdownHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerKnockDownProfile"));
}

void ARedCCharacter::SwitchHitBoxStateToStanding_Implementation()
{
	//UE_LOG(LogRedC, Log, TEXT("Switch Collision Standing"));
	StandingHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KnockdownHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
#pragma endregion

#pragma region Motion Warping
UContextualAnimSceneActorComponent* ARedCCharacter::GetContextualAnimSceneActorComponent() const
{
	return ContextualAnimSceneActorComp;
}

void ARedCCharacter::ServerStopAllMontage_Implementation(float BlendOutDuration)
{
	PrepareCharacterNextAnim(BlendOutDuration);
}

void ARedCCharacter::PrepareCharacterNextAnim_Implementation(float BlendOutDuration)
{
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->StopAllMontages(BlendOutDuration);
	}
}

UMotionWarpingComponent* ARedCCharacter::GetMotionWarpingComponent() const
{
	return MotionWarpingComp;
}

void ARedCCharacter::SetMotionWarpingTarget(FMotionWarpingParam& MotionWarpingParam)
{
	MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(
		MotionWarpingParam.WarpingName,
		MotionWarpingParam.WarpingLocation,
		MotionWarpingParam.WarpingRotation
	);
}

void ARedCCharacter::RemoveAllMotionWarpingTarget()
{
	MotionWarpingComp->RemoveAllWarpTargets();
}

void ARedCCharacter::ServerRunMotionWarping_Implementation(UContextualAnimSceneAsset* Asset, AActor* TargetActor, UAnimMontage* Montage)
{
	// 우선, Blueprint에 나왔던 Binding Context 생성하는 부분
	TMap<FName, FContextualAnimSceneBindingContext> BindingContexts;
	BindingContexts.Add(TEXT("Attacker"), FContextualAnimSceneBindingContext(this));
	BindingContexts.Add(TEXT("Defender"), FContextualAnimSceneBindingContext(TargetActor));

	ARedCCharacter* Target = Cast<ARedCCharacter>(TargetActor);
	Target->PrepareCharacterNextAnim();
	
	// 만든 Context를 Scene에 바인딩
	FContextualAnimSceneBindings SceneBindings;
	bool bSuccess = UContextualAnimUtilities::BP_CreateContextualAnimSceneBindings(
		Asset,
		BindingContexts,
		SceneBindings
	);

	if (bSuccess)
	{
		//UE_LOG(LogRedCGA, Warning, TEXT("OnGrabTarget : Scene Bindings Success"));
		ContextualAnimSceneActorComp->StartContextualAnimScene(SceneBindings);

		// 잡기는 넉다운 시간을 초기화함.
		Target->CancelGetUpTimer();
		
		Target->StartGetUpTimer();
		Target->SwitchHitBoxStateToKnockdown();
	}
	else
	{
		UE_LOG(LogRedCGA, Warning, TEXT("OnGrabTarget : Scene Bindings Failed"));
	}
}
#pragma endregion

#pragma region Rotation Locking & Force Setting Rotation
void ARedCCharacter::ForceSetCharacterRotation_Implementation(FRotator NewRotation)
{
	SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
}

void ARedCCharacter::UnlockControllerRotation_Implementation()
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetNetMode() == NM_DedicatedServer)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;	
	}
}

void ARedCCharacter::LockControllerRotation_Implementation()
{
	if (GetLocalRole() == ROLE_AutonomousProxy  || GetNetMode() == NM_DedicatedServer)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}
#pragma endregion

#pragma region Get-Up(Recovery Battle State)
void ARedCCharacter::StartGetUpTimer()
{
	if (GetUpTimerHandle.IsValid())
	{
		if (GetAbilitySystemComponent()->HasMatchingGameplayTag(PlayerStateTags::Player_State_Combat_KnockedDown))
		{
			return;
		}
		CancelGetUpTimer();
	}

	GetUpStartTime = GetWorld()->GetTimeSeconds();
	RemainTime = GetUpTimerDuration;
	GetWorld()->GetTimerManager().SetTimer(
		GetUpTimerHandle,
		this,
		&ARedCCharacter::GetUpTimerCallback,
		0.1f,
		true
	);
}

void ARedCCharacter::GetUpTimerCallback()
{
	if (IsCharacterHasTag(PlayerStateTags::Player_State_Death))
	{
		GetWorld()->GetTimerManager().ClearTimer(GetUpTimerHandle);
	}
	
	if (GetMovementComponent()->IsMovingOnGround())
	{
		RemainTime = RemainTime - 0.1f;
		//UE_LOG(LogRedC, Log, TEXT("GetUpTimerCallback : RemainTime : %f"), RemainTime);
		if (RemainTime <= 0.0f)
		{
			if (IsValid(GetUpMontage))
			{
				MulticastPlayMontage(GetUpMontage);
			}
			else
			{
				UE_LOG(LogRedC, Error, TEXT("GetUpMontage is not valid."));
			}
			FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle EffectSpec = GetAbilitySystemComponent()->MakeOutgoingSpec(
				UInvincibleEffect::StaticClass(),
				1.0f,
				EffectContext
			);

			if (EffectSpec.IsValid())
			{
				GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
			}
			
			GetWorld()->GetTimerManager().ClearTimer(GetUpTimerHandle);
			SwitchHitBoxStateToStanding();
			CombatSystem->ClearHitMessageContainer();
			LockControllerRotation();
		}
	}
}

void ARedCCharacter::CancelGetUpTimer()
{
	if (GetUpTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GetUpTimerHandle);
	}
}

void ARedCCharacter::MulticastPlayMontage_Implementation(UAnimMontage* Montage)
{
	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		AnimInst->StopAllMontages(0.25f);
	}
	
	if (IsValid(Montage))
	{
		PlayAnimMontage(Montage);	
	}
}
#pragma endregion

#pragma region Hit React
void ARedCCharacter::ServerReceiveHitReactData_Implementation(FHitReactMessage Message, AActor* HitActor)
{
	// 추가 로직이 필요할 경우 하위 클래스에서 작성하기 바람
	// Player의 경우 RedCHeroCharacter에서
	// AI의 경우 RedCAICharacter에서 ServerGetHitReactData가 아닌 _Implementation 함수를 오버라이딩 할 것.
	if (ARedCCharacter* HitCharacter = Cast<ARedCCharacter>(HitActor))
	{
		if (UCombatComponent* Comp = HitCharacter->GetCombatComponent())
		{
			Comp->PushHitMessage(Message);
			Comp->ServerApplyDamageToOwner(Message, this);
			Comp->ExecuteGameplayCue(Message, this);
		}
		else
		{
			UE_LOG(LogRedCServer, Error, TEXT("Combat Component is not found."));
		}
	}
}


// 기존 HeroCharacter에 있던 코드에서 상위 클래스인 RedCCharacter로 코드를 옮김
// 추가로, 384 Byte 정도 쓰던 크기를 80 Byte로 줄임.
void ARedCCharacter::ClientActivateHitReact_Implementation(const FGameplayTag& HitStrength, FHitReactMessage Message)
{
	// 위 ServerGetHitReactData()와 마찮가지.
	ProcessHitReaction(HitStrength);
}

void ARedCCharacter::ProcessHitReaction(const FGameplayTag& HitStrength)
{
	//UE_LOG(LogRedC, Log, TEXT("8. Enter Process Hit Reaction"));
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer(HitStrength), true);
}

void ARedCCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ARedCCharacter, CachedTeamId, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ARedCCharacter, IsDead, COND_None, REPNOTIFY_OnChanged);
}

#pragma endregion

void ARedCCharacter::DeathTagUpdated(FGameplayTag GameplayTag, int32 Count)
{
	if (Count != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void ARedCCharacter::RemovePotionEffects_OnDeath()
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this))
	{
		FGameplayTagContainer T;
		T.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Effect.Item.Potion")));
		const int32 Removed = ASC->RemoveActiveEffectsWithGrantedTags(T);
		UE_LOG(LogTemp, Warning, TEXT("[Death] Removed potion GEs(by tag): %d"), Removed);
	}
}

void ARedCCharacter::StartDeathSequence()
{
	OnDeath();
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	/*
	 *	 여긴 클라도 실행함.
	 *
	 */
	if (HasAuthority())
	{
		//MulticastPlayMontage(DeathMontage);
		IsDead = true;
		OnRep_Dead();
		SwitchHitBoxStateToKnockdown();
		CombatSystem->ClearHitMessageContainer();
		//캐릭터에적용된 아이템효과제거 ge
		RemovePotionEffects_OnDeath();
	}
	else 
	{
		if (ARedCPlayerController* PC = Cast<ARedCPlayerController>(GetController()))
		{
			PC->ShowRespawncount();
		}
		OnCharacterDead.Broadcast(GetPlayerState<APlayerStateArena>()->CharacterLevel * 0.8f + 7.f);
	}
}

void ARedCCharacter::Respawn()
{
	OnRespawn();
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	GetAbilitySystemComponent()->RemoveLooseGameplayTag(PlayerStateTags::Player_State_Combat_KnockedDown);
	if (HasAuthority())
	{
		if(AGameModeArena* GM = GetWorld()->GetAuthGameMode<AGameModeArena>())
		{
			FTransform RespawnPoint = GM->GetRespawnLocation(this);
			SetActorRotation(RespawnPoint.GetRotation());
			ClientRespawn(RespawnPoint.GetRotation().Rotator());
			SetActorLocation(RespawnPoint.GetLocation(), false, nullptr, ETeleportType::ResetPhysics);
		}
		SwitchHitBoxStateToStanding();
		//MulticastPlayMontage(RespawnMontage);
		IsDead = false;
		OnRep_Dead();
		CombatSystem->ClearHitMessageContainer();
	}
	else
	{
		if (ARedCPlayerController* PC = Cast<ARedCPlayerController>(GetController()))
		{
			PC->HiddenRespawncount();
		}
	}
}

void ARedCCharacter::OnRep_Dead()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.25f);
		}
	}
	
	if (IsDead && DeathMontage)
	{
		PlayAnimMontage(DeathMontage);	
	}
	else if (not IsDead && RespawnMontage)
	{
		PlayAnimMontage(RespawnMontage);
	}
}

void ARedCCharacter::ClientRespawn_Implementation(FRotator Rotation)
{
	GetController()->SetControlRotation(Rotation);
}

FString ARedCCharacter::GetActorLocalRoleName() const
{
	FString localRole = FString();
	switch (GetLocalRole())
	{
	case ROLE_Authority:
		localRole = "Authority";
		break;
	case ROLE_AutonomousProxy:
		localRole = "AutonomousProxy";
		break;
	case ROLE_SimulatedProxy:
		localRole = "SimulatedProxy";
		break;
	case ROLE_None:
		localRole = "None";
		break;
	default:
		localRole = "Unknown";
	}
	return localRole;
}

FGameplayTag ARedCCharacter::GetCharacterTag() const
{
	return CharacterTag;
}

uint8 ARedCCharacter::GetCharacterTeamId_UINT8() const
{
	return CachedTeamId.GetId();
}

bool ARedCCharacter::IsCharacterSameTeam(const ICharacterIdentifier* Other) const
{
	if (CachedTeamId.GetId() == Other->GetCharacterTeamId_UINT8())
	{
		return true;
	}
	return false;
}

void ARedCCharacter::UpdateCachedTeamId(FGenericTeamId NewTeamId)
{
	CachedTeamId = NewTeamId;
}

void ARedCCharacter::OnRep_CachedTeamId()
{
	// 자식에서 구현 (Super 함수 호출 X)
}
