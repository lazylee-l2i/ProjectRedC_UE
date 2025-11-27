// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "RedCAbilitySystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCAbilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ExecuteGameplayCue(const FGameplayTag& GameplayCueTag, AActor* Target);
};
