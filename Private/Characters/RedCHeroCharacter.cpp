// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RedCHeroCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/CharacterDefinition.h"
#include "Characters/Components/CrosshairComponent.h"
#include "Characters/Components/MovementEffectManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Controllers/RedCPlayerController.h"
#include "Core/Systems/RedCAssetManager.h"
#include "Frameworks/GameMode/GameModeArena.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RedCAbilitySet.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "GAS/Attributes/RedCEnergySet.h"
#include "GAS/Attributes/RedCMovementSet.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/RedCFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Types/RedCCharacterTypes.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"
#include "UI/Widget/Battle/OverheadInfoWidget.h"
#include "Utility/LogHelper.h"


ARedCHeroCharacter::ARedCHeroCharacter()
	: CharacterDefinition(nullptr), AbilitySystemComp(nullptr), CachedMaxStamina(0.f)
{
	CrosshairComp = CreateDefaultSubobject<UCrosshairComponent>(TEXT("CrosshairComponent"));
	MovementEffectManagerComp = CreateDefaultSubobject<UMovementEffectManagerComponent>(TEXT("MovementEffectManagerComponent"));
	
	IdentificationComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IdentificationCompComponent"));
	IdentificationComp->bReceivesDecals = false;
	IdentificationComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IdentificationComp->SetVisibility(false);
	IdentificationComp->SetupAttachment(GetMesh());

	OverheadInfoWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadInfoWidgetComponent"));
	OverheadInfoWidgetComp->SetHiddenInGame(true);
	OverheadInfoWidgetComp->SetupAttachment(GetRootComponent());
}

void ARedCHeroCharacter::ResetActorPosition()
{
	if (AGameModeArena* GM = Cast<AGameModeArena>(GetWorld()->GetAuthGameMode()))
	{
		FTransform Transform = GM->GetRespawnLocation(this);
		SetActorLocation(Transform.GetLocation());
		SetActorRotation(Transform.GetRotation());
	}
}

void ARedCHeroCharacter::PlayerStateTagsUpdated(const FGameplayTag GameplayTag, const int32 Count)
{
	if (!GameplayTag.IsValid())
	{
		return;
	}

	if (Count != 0)
	{
		if (GameplayTag == PlayerStateTags::Player_State_Death)
		{
			IdentificationComp->SetVisibility(false);
			UpdateOverheadInfoWidgetVisibility(true);
		}
		
		if (GameplayTag == PlayerStateTags::Player_State_Invincible)
		{
			SetMeshOverlayMaterial(true);
		}
	}
	else
	{
		if (GameplayTag == PlayerStateTags::Player_State_Death)
		{
			InitIdentificationComponent();
			UpdateOverheadInfoWidgetVisibility(false);
		}
		
		if (GameplayTag == PlayerStateTags::Player_State_Invincible)
		{
			SetMeshOverlayMaterial(false);
		}
	}
}

void ARedCHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() && !IsNetMode(NM_DedicatedServer))
	{
		if (CrosshairComp)
		{
			CrosshairComp->CreateCrosshairWidget();
		}
	}
}

void ARedCHeroCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsLocallyControlled() && !IsNetMode(NM_DedicatedServer))
	{
		if (CrosshairComp)
		{
			CrosshairComp->RemoveCrosshairWidget();
		}
	}
}

void ARedCHeroCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateOverheadInfoWidgetRotation();
}

void ARedCHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ResetActorPosition();
	InitGenericTeamId();
	
	ResolveCharacterDefinition();
	SetupCharacterMeshComponent();
	InitAbilitySystemComponent();
}

void ARedCHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	ResolveCharacterDefinition();
	SetupCharacterMeshComponent();
	InitAbilitySystemComponent();
}

void ARedCHeroCharacter::ApplyStartupAbilitiesAndAttributes_Implementation()
{
	const APlayerStateArena* PS = GetPlayerState<APlayerStateArena>();
	if (not PS)
	{
		return;
	}
	if (AbilitySystemComp->GetOwnerActor() != PS || AbilitySystemComp->GetAvatarActor() != this)
	{
		//LogHelper::Print(LogRedC, ELogVerbosity::Warning, "Ability System Component Initialization failed.", this, false);
		return;
	}

	if (CharacterDefinition->CharacterAbilitySet)
	{
		// 캐릭터 고유 어빌리티 추가
		AbilitySystemComp->GrantGameplayAbilities(CharacterDefinition->CharacterAbilitySet->GetAbilities());
	}

	if (URedCAbilitySet* AbilitySet = URedCAssetManager::GetAssetByTag<URedCAbilitySet>(AssetTags::Asset_Character_CommonAbilitySet))
	{
		// 캐릭터 공통 어빌리티 추가
		AbilitySystemComp->GrantGameplayAbilities(AbilitySet->GetAbilities());

		// 캐릭터 고유 능력치 적용
		{
			UDataTable* DataTable = URedCAssetManager::GetAssetByTag<UDataTable>(AssetTags::Asset_Character_CharacterStats);
			const FCharacterStatInfo* Data = URedCFunctionLibrary::GetDataTableRowByTag<FCharacterStatInfo>(DataTable, CharacterTag);
			
			const FRedCEffectInfo* Effect = AbilitySet->EffectMap.Find(PlayerEffectTags::Player_Effect_Start);
			const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComp->MakeOutgoingSpec(
				Effect->EffectClass, Effect->EffectLevel, AbilitySystemComp->MakeEffectContext());

			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTags::Data_SetByCaller_AttackPower, Data->AttackPower);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTags::Data_SetByCaller_Critical, Data->Critical);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTags::Data_SetByCaller_Defense, Data->Defense);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTags::Data_SetByCaller_Evasion, Data->Evasion);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTags::Data_SetByCaller_MaxHealth, Data->MaxHealth);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTags::Data_SetByCaller_MaxStamina, Data->MaxStamina);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTags::Data_SetByCaller_MovementSpeed, Data->MovementSpeed);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTags::Data_SetByCaller_Coin, 3200.f);

			AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		
		// 캐릭터 체력 & 스태미나 적용
		ApplyRespawnEffect();
		CachedMaxStamina = PS->GetRedCEnergySet()->GetMaxStamina();
	}
}

void ARedCHeroCharacter::ResolveCharacterDefinition()
{
	Super::ResolveCharacterDefinition();

	FString CharacterName;
	CharacterTag.ToString().Split(TEXT("."), nullptr, &CharacterName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	const FGameplayTag CharacterAssetTag = URedCFunctionLibrary::FindChildTagByName(CharacterName, AssetTags::Asset_Character_Definition);
	if (!CharacterAssetTag.IsValid())
	{
		//LogHelper::Print(LogRedC, ELogVerbosity::Warning,
		//	FString::Printf(TEXT("Character Name [%s] or Asset Tag [%s] is not valid."), *CharacterName, *CharacterAssetTag.ToString()));
		return;
	}
	
	CharacterDefinition = URedCAssetManager::GetAssetByTag<UCharacterDefinition>(CharacterAssetTag);
	ensureMsgf(CharacterDefinition, TEXT("Character Definition is not valid."));
}

void ARedCHeroCharacter::SetupCharacterMeshComponent()
{
	GetMesh()->SetSkeletalMesh(CharacterDefinition->CharacterMeshInfo.CharacterMesh);
	GetMesh()->SetAnimInstanceClass(CharacterDefinition->CharacterMeshInfo.CharacterAnimClass);

	for (const FCharacterAdditiveMeshInfo& AdditiveMesh : CharacterDefinition->CharacterMeshInfo.CharacterAdditiveMeshes)
	{
		if (UStaticMeshComponent* AdditiveMeshComp = NewObject<UStaticMeshComponent>(this))
		{
			AdditiveMeshComp->SetStaticMesh(AdditiveMesh.StaticMesh);
			AdditiveMeshComp->SetMobility(EComponentMobility::Movable);
			AdditiveMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
			AdditiveMeshComp->RegisterComponent();
			AdditiveMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), AdditiveMesh.SocketName);
		}
	}
}

void ARedCHeroCharacter::InitAbilitySystemComponent()
{
	APlayerStateArena* PS = GetPlayerState<APlayerStateArena>();
	if (!PS)
	{
		//LogHelper::Print(LogRedC, ELogVerbosity::Warning, "Player State is not valid.", this, false);
		return;
	}

	URedCPlayerAbilitySystemComponent* ASC = PS->GetPlayerAbilitySystemComponent();
	if (!ASC)
	{
		//LogHelper::Print(LogRedC, ELogVerbosity::Warning, "Player ASC is not valid.", this, false);
		return;
	}

	AbilitySystemComp = ASC;
	AbilitySystemComp->InitAbilityActorInfo(PS, this);

	if (IsValid(AbilitySystemComp))
	{
		MovementEffectManagerComp->RegisterOwnerASC(AbilitySystemComp);

		BindCallbacksToDependencies();
		ConfigureOverheadHealthWidget();
		ConfigureOverheadInfoWidget();

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ApplyStartupAbilitiesAndAttributes();
		}
	}
}

void ARedCHeroCharacter::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	
	if (IsValid(AbilitySystemComp))
	{
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(URedCMovementSet::GetMovementSpeedAttribute()).AddUObject(this, &ThisClass::OnSpeedChanged);
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(URedCEnergySet::GetStaminaAttribute()).AddUObject(this, &ThisClass::OnStaminaChanged);

		AbilitySystemComp->RegisterGameplayTagEvent(PlayerStateTags::Player_State_Invincible).AddUObject(this, &ThisClass::PlayerStateTagsUpdated);
		AbilitySystemComp->RegisterGameplayTagEvent(PlayerStateTags::Player_State_Death).AddUObject(this, &ThisClass::PlayerStateTagsUpdated);
	}
}

void ARedCHeroCharacter::InitGenericTeamId()
{
	if (const IGenericTeamAgentInterface* PS = Cast<IGenericTeamAgentInterface>(GetPlayerState()))
	{
		UpdateCachedTeamId(PS->GetGenericTeamId());
		//UE_LOG(LogRedC, Warning, TEXT("Role %s, TeamID %d, CharacterName %s"), *GetActorLocalRoleName(), GetCharacterTeamId_UINT8(), *GetName());
		ApplyTeamCollision();
	}
}

void ARedCHeroCharacter::ApplyRespawnEffect()
{
	if (URedCAbilitySet* AbilitySet = URedCAssetManager::GetAssetByTag<URedCAbilitySet>(AssetTags::Asset_Character_CommonAbilitySet))
	{
		const FRedCEffectInfo* Effect = AbilitySet->EffectMap.Find(PlayerEffectTags::Player_Effect_Respawn);
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComp->MakeOutgoingSpec(
			Effect->EffectClass, Effect->EffectLevel, AbilitySystemComp->MakeEffectContext());

		AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ARedCHeroCharacter::InitIdentificationComponent()
{
	if (IsLocallyControlled())
	{
		return;
	}

	check(IdentificationComp);
	
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		const ARedCHeroCharacter* Character = PC->GetPawn<ARedCHeroCharacter>();
		if (!Character) return;
		
		IdentificationComp->SetVisibility(!IsCharacterSameTeam(Character));
	}
}

void ARedCHeroCharacter::ApplyTeamCollision()
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
}

void ARedCHeroCharacter::SetMeshOverlayMaterial(bool Enable)
{
	UMaterialInterface* MaterialInterface = Enable ? OverlayMaterial : nullptr;
	
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		// 캐릭터 몸체 Mesh에 OverlayMaterial 적용
		MeshComponent->SetOverlayMaterial(MaterialInterface);

		// 하위 MeshComponent 찾기
		TArray<UMeshComponent*> ChildMeshComponents;
		GetComponents(ChildMeshComponents);

		if (IsValid(IdentificationComp))
		{
			ChildMeshComponents.Remove(IdentificationComp);
		}

		// 하위 MeshComponent에 OverlayMaterial 적용
		for (UMeshComponent* ChildMeshComp : ChildMeshComponents)
		{
			if (Cast<UStaticMeshComponent>(ChildMeshComp))
			{
				ChildMeshComp->SetOverlayMaterial(MaterialInterface);
			}
		}
	}
}

void ARedCHeroCharacter::ServerApplyGameplayEffect_Implementation(FGameplayTag EffectTag) const
{
	MovementEffectManagerComp->ApplyMovementGameplayEffectByTag(EffectTag);
}

void ARedCHeroCharacter::ServerRemoveGameplayEffect_Implementation(FGameplayTag EffectTag) const
{
	MovementEffectManagerComp->RemoveMovementEffectByTag(EffectTag);
}

void ARedCHeroCharacter::ServerRemoveMovementEffect_Implementation() const
{
	MovementEffectManagerComp->EraseMovementEffect();
}

URedCPlayerAbilitySystemComponent* ARedCHeroCharacter::GetRedCPlayerAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

UAbilitySystemComponent* ARedCHeroCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

UCrosshairComponent* ARedCHeroCharacter::GetCrosshairComponent_Implementation() const
{
	return CrosshairComp;
}

void ARedCHeroCharacter::OnRep_CachedTeamId()
{
	InitIdentificationComponent();
	ApplyTeamCollision();
}

void ARedCHeroCharacter::OnDeath()
{
	Super::OnDeath();
}

void ARedCHeroCharacter::OnRespawn()
{
	Super::OnRespawn();
	ApplyRespawnEffect();
}


void ARedCHeroCharacter::SwitchHitBoxStateToStanding_Implementation()
{
	Super::SwitchHitBoxStateToStanding_Implementation();
	ApplyTeamCollision();
}

bool ARedCHeroCharacter::IsInvincible() const
{
	return IsCharacterHasTag(PlayerStateTags::Player_State_Invincible);
}

void ARedCHeroCharacter::SetForwardInput_Implementation(bool bIsPressingForward)
{
	this->bForwardPressing = bIsPressingForward;
}

void ARedCHeroCharacter::SetBackwardInput_Implementation(bool bIsPressingBackward)
{
	this->bBackwardPressing = bIsPressingBackward;
}

uint8 ARedCHeroCharacter::GetCurrentMovementEffect()
{
	//UE_LOG(LogRedC, Warning, TEXT("Current Input State => Foward : %d, Backward : %d"), bFowardPressing, bBackwardPressing);
	if (bForwardPressing)
	{
		return 1;
	}
	else if (bBackwardPressing)
	{
		return 2;
	}
	else
	{
		return 0;
	}
}

void ARedCHeroCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARedCHeroCharacter, bForwardPressing);
	DOREPLIFETIME(ARedCHeroCharacter, bBackwardPressing);
}

void ARedCHeroCharacter::OnSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	GetCharacterMovement()->MaxWalkSpeed = OnAttributeChangeData.NewValue;
}

void ARedCHeroCharacter::OnStaminaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UKismetMathLibrary::Clamp(OnAttributeChangeData.NewValue, 0.f, CachedMaxStamina);
	//UE_LOG(LogRedC, Warning, TEXT("New Stamina Value: %f, Max Stamina: %f"), OnAttributeChangeData.NewValue, CachedMaxStamina);
	if (OnAttributeChangeData.NewValue >= CachedMaxStamina)
	{
		//UE_LOG(LogRedC, Warning, TEXT("Stamina Reached Max Value: %f"), CachedMaxStamina);
		MovementEffectManagerComp->RemoveMovementEffectByTag(PlayerEffectTags::Player_Effect_Buff_RegenStamina);
	}

	if (OnAttributeChangeData.NewValue <= 0.f)
	{
		//MovementEffectManagerComp->ApplyMovementGameplayEffectByTag(PlayerEffectTags::Player_Effect_Speed_Sprint);
		AbilitySystemComp->AbilityInputReleased(InputTags::Input_Action_Ability_Sprint);	
	}
}

void ARedCHeroCharacter::ConfigureOverheadInfoWidget()
{
	check(OverheadInfoWidgetComp);

	if (GetController() && GetController()->IsLocalPlayerController())
	{
		OverheadInfoWidgetComp->SetHiddenInGame(true);
		return;
	}

	OverheadInfoWidgetComp->SetHiddenInGame(false);

	UOverheadInfoWidget* OverheadInfoWidget = Cast<UOverheadInfoWidget>(OverheadInfoWidgetComp->GetUserWidgetObject());
	if (!OverheadInfoWidget) return;

	if (APlayerStateArena* PS = GetPlayerState<APlayerStateArena>())
	{
		PS->OnPrivatePlayerNameChanged.AddLambda([this, OverheadInfoWidget](const FName PrivatePlayerName)
		{
			OverheadInfoWidget->SetPlayerNameText(PrivatePlayerName);
		});
		
		PS->OnCharacterLevelChanged.AddLambda([this, OverheadInfoWidget](const int32 Level)
		{
			OverheadInfoWidget->SetLevelText(Level);
		});

		PS->OnCharacterNameChanged.AddLambda([this, OverheadInfoWidget](const FName CharacterName)
		{
			OverheadInfoWidget->SetCharacterNameText(CharacterName);
		});

		PS->BroadcastPlayerInfo();
	}

	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		const ARedCHeroCharacter* Character = PC->GetPawn<ARedCHeroCharacter>();
		if (!Character) return;
		
		OverheadInfoWidget->SetTeamColor(IsCharacterSameTeam(Character));
	}
}

void ARedCHeroCharacter::UpdateOverheadInfoWidgetRotation()
{
	if (IsLocallyControlled()) return;
	
	check(OverheadInfoWidgetComp);

	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		const FVector ForwardVector = PC->GetControlRotation().Vector();
		const FRotator Rotation = FRotationMatrix::MakeFromX(ForwardVector * -1.f).Rotator();

		OverheadInfoWidgetComp->SetWorldRotation(Rotation);
	}
}

void ARedCHeroCharacter::UpdateOverheadInfoWidgetVisibility(bool bShouldHide)
{
	if (IsLocallyControlled()) return;
	
	check(OverheadInfoWidgetComp);
	
	OverheadInfoWidgetComp->SetHiddenInGame(bShouldHide);
}
