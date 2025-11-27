// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/RedCAbilitySystemGlobals.h"

#include "Types/RedCAbilityTypes.h"


FGameplayEffectContext* URedCAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FCustomGameplayEffectContext();
}
