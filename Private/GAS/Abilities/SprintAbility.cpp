// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SprintAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Characters/RedCHeroCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Attributes/RedCEnergySet.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"


bool USprintAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                        const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (GetAbilitySystemComponentFromActorInfo()->GetNumericAttributeBase(URedCEnergySet::GetStaminaAttribute()) < 1.f)
	{
		UE_LOG(LogRedC, Error, TEXT("Not Enough Stamina to Sprint."));
		return false;
	}
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}


void USprintAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (not CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (ARedCCharacter* OwnerCharacter = Cast<ARedCCharacter>(GetAvatarActorFromActorInfo()))
		{
			OwnerCharacter->UnlockControllerRotation();
			/*Movement->bOrientRotationToMovement = true;
			Cast<APawn>(GetAvatarActorFromActorInfo())->bUseControllerRotationYaw = false;*/
		}

		{
			UAbilityTask_WaitInputRelease* InputTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
			InputTask->OnRelease.AddDynamic(this, &ThisClass::OnReleased);
			InputTask->ReadyForActivation();
			AddWatchedTask(InputTask);
		}
		
		{
			ARedCHeroCharacter* PC = Cast<ARedCHeroCharacter>(GetAvatarActorFromActorInfo());
			PC->ServerRemoveGameplayEffect(PlayerEffectTags::Player_Effect_Buff_RegenStamina);
			PC->ServerApplyGameplayEffect(PlayerEffectTags::Player_Effect_Speed_Sprint);
			PC->ServerApplyGameplayEffect(PlayerEffectTags::Player_Effect_Debuff_DrainStamina);
		}
	}
}

void USprintAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	OnEndAbility();
}

void USprintAbility::OnReleased(float TimeHeld)
{
	OnEndAbility();
}

void USprintAbility::OnEndAbility()
{
	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		MakePlayerDefaultState(AvatarActor);
	}
	Super::OnEndAbility();
}

void USprintAbility::MakePlayerDefaultState(AActor* AvatarActor) const
{
	if (!AvatarActor || !AvatarActor->IsValidLowLevel() || AvatarActor->IsPendingKillPending())
	{
		return;
	}

	ARedCHeroCharacter* PlayerCharacter = Cast<ARedCHeroCharacter>(AvatarActor);
	if (!PlayerCharacter) return;

	PlayerCharacter->ServerRemoveGameplayEffect(PlayerEffectTags::Player_Effect_Speed_Sprint);
	PlayerCharacter->ServerRemoveGameplayEffect(PlayerEffectTags::Player_Effect_Debuff_DrainStamina);

	PlayerCharacter->ServerApplyGameplayEffect(PlayerEffectTags::Player_Effect_Buff_RegenStamina);

	PlayerCharacter->LockControllerRotation();
	/*if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = false;
		PlayerCharacter->bUseControllerRotationYaw = true;
	}*/
	
}


	
