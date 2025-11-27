// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/RedCAbilitySystemComponent.h"
#include "RedCAIAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCAIAbilitySystemComponent : public URedCAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void InitializeAbilitiesAndEffectsSafe(
		const TArray<TSubclassOf<UGameplayAbility>>& Abilities, const TArray<TSubclassOf<UGameplayEffect>>& Effects);
};
