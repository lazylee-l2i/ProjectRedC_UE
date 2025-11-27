// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Types/RedCAbilityTypes.h"
#include "RedCAbilitySet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCAbilitySet : public UDataAsset
{
	GENERATED_BODY()
	
public:
	TArray<FRedCAbilityInfo> GetAbilities();
	TArray<FGameplayTag> GetAbilityTags();
	FORCEINLINE TMap<FGameplayTag, FRedCAbilityInfo> GetAbilityMap() { return AbilityMap; }
	TArray<FRedCEffectInfo> GetEffects();
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FRedCAbilityInfo> AbilityMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FRedCEffectInfo> EffectMap;
};
