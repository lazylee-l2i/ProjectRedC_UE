// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/RedCAbilitySet.h"


TArray<FRedCAbilityInfo> URedCAbilitySet::GetAbilities()
{
	TArray<FRedCAbilityInfo> Abilities = TArray<FRedCAbilityInfo>();

	for (const auto& Ability : AbilityMap)
	{
		Abilities.Add(Ability.Value);
	}

	return Abilities;
}

TArray<FGameplayTag> URedCAbilitySet::GetAbilityTags()
{
	TArray<FGameplayTag> Tags = TArray<FGameplayTag>();
	for (const auto& Ability : AbilityMap)
	{
		Tags.Add(Ability.Key);
	}
	return Tags;
}

TArray<FRedCEffectInfo> URedCAbilitySet::GetEffects()
{
	TArray<FRedCEffectInfo> Effects = TArray<FRedCEffectInfo>();

	for (const auto& Effect : EffectMap)
	{
		Effects.Add(Effect.Value);
	}
	
	return Effects;
}
