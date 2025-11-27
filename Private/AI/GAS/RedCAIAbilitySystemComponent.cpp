// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAS/RedCAIAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/Abilities/RedCGameplayAbility.h"

void URedCAIAbilitySystemComponent::InitializeAbilitiesAndEffectsSafe(
	const TArray<TSubclassOf<UGameplayAbility>>& Abilities,
	const TArray<TSubclassOf<UGameplayEffect>>& Effects)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;
	
	if (!IsValid(this))
	{
		return;
	}

	InitAbilityActorInfo(Cast<APawn>(GetOwner()), GetOwner());
	
	// Ability 초기화
	for (const TSubclassOf<UGameplayAbility>& Ability : Abilities)
	{
		
		FGameplayAbilitySpec AbilitySpec(Ability, 1.f);
		if (const auto GameplayAbility = Cast<URedCGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(GameplayAbility->InputTag);
		}
		GiveAbility(AbilitySpec);
	}

	// Effect 초기화
	for (const TSubclassOf<UGameplayEffect>& Effect : Effects)
	{
		if (!Effect) 
		{
			continue;
		}

		const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Effect, 1.f, ContextHandle);

		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Invalid EffectSpec: %s"), *Effect->GetName());
		}
	}
}
