// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/WheelUpAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/RedCHeroCharacter.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"


UWheelUpAbility::UWheelUpAbility()
{
	AbilityTags.AddTag(PlayerAbilityTags::Player_Ability_Buff_WheelUp);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UWheelUpAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsValid(WheelUpEffect))
	{
		if (WheelUpEffect->GetDefaultObject<UGameplayEffect>()->DurationPolicy == EGameplayEffectDurationType::HasDuration)
		{
			if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
			{
				BP_ApplyGameplayEffectToOwner(WheelUpEffect);
				UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
					this,
					NAME_None,
					WheelUpMontage,
					1.f
				);
				PlayTask->OnCancelled.AddDynamic(this, &UWheelUpAbility::OnEndAbility);
				PlayTask->OnBlendOut.AddDynamic(this, &UWheelUpAbility::OnEndAbility);
				PlayTask->ReadyForActivation();
				AddWatchedTask(PlayTask);
			}	
		}
		else
		{
			UE_LOG(LogRedC, Error, TEXT("WheelUpEffect DurationPolicy is not HasDuration. %s will not working."), *this->GetName());
		}
	}
}
