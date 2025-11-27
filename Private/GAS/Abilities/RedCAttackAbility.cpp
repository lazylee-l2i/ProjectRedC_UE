// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/RedCAttackAbility.h"

#include <functional>

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GAS/Effects/ApplyDamageEffect.h"
#include "GAS/Effects/SkillCoolDownEffect.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"

URedCAttackAbility::URedCAttackAbility()
{
	CancelAbilitiesWithTag.AddTag(PlayerAbilityTags::Player_Ability_Movement);
	CancelAbilitiesWithTag.AddTag(PlayerAbilityTags::Player_Ability_Buff_WheelUp);
	
	BlockAbilitiesWithTag.AddTag(PlayerAbilityTags::Player_Ability_Attack);
	BlockAbilitiesWithTag.AddTag(PlayerAbilityTags::Player_Ability_Movement);
	BlockAbilitiesWithTag.AddTag(PlayerAbilityTags::Player_Ability_Buff_WheelUp);

	ActivationOwnedTags.AddTag(PlayerStateTags::Player_State_Lock_Look);
	ActivationOwnedTags.AddTag(PlayerStateTags::Player_State_Lock_Move);
	
	CooldownGameplayEffectClass = USkillCoolDownEffect::StaticClass();
}

void URedCAttackAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(USkillCoolDownEffect::StaticClass(), GetAbilityLevel(Handle, ActorInfo));

	SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(FGameplayTagContainer(InputTag));
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_CoolDown, CoolDownDuration);
	
	//ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

float URedCAttackAbility::GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const
{
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(InputTag));
	//UE_LOG(LogRedC, Warning, TEXT("CanActivateAbility InputTag = %s"), *InputTag.ToString());
	
	TArray<TPair<float, float>> DurationAndTimeRemaining = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(Query);
	if (DurationAndTimeRemaining.Num() > 0)
	{
		for (TPair<float, float> Item : DurationAndTimeRemaining)
		{
			//UE_LOG(LogRedC, Warning, TEXT("CanActivateAbility key = %f, value = %f"), Item.Key, Item.Value);
			return Item.Key;
		}
	}
	return Super::GetCooldownTimeRemaining(ActorInfo);
}

bool URedCAttackAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                            const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{	
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(InputTag))
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void URedCAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	#pragma region Initial Setting & Check Commit
	if (not K2_CommitAbility())
	{
		K2_EndAbility();
	}
	#pragma endregion
	
}

void URedCAttackAbility::OnReleased(float TimeHeld)
{
	K2_EndAbility();
}


void URedCAttackAbility::ApplyDamageEffect(AActor* HitResult, float DamageAmount)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
		UApplyDamageEffect::StaticClass(),
		GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo())
	);

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(
			DataTags::Data_SetByCaller_Health,
			-DamageAmount
		);

		ApplyGameplayEffectSpecToTarget(
			GetCurrentAbilitySpecHandle(), 
			CurrentActorInfo, 
			CurrentActivationInfo, 
			SpecHandle, 
			UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult)
		);
	}
}
