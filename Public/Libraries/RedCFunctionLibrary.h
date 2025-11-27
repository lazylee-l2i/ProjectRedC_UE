// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/RedCAbilityTypes.h"
#include "RedCFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FGameplayTag FindChildTagByName(const FString& Name, const FGameplayTag& ParentTag);
	
	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag Tag);

	UFUNCTION(BlueprintPure)
	static FVector GetCustomVector(FGameplayEffectContextHandle EffectContextHandle);

	UFUNCTION(BlueprintPure)
	static EWeaponType GetWeaponType(FGameplayEffectContextHandle EffectContextHandle);

	UFUNCTION(BlueprintPure)
	static EArmorType GetArmorType(FGameplayEffectContextHandle EffectContextHandle);
};

template <typename T>
T* URedCFunctionLibrary::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), Tag.ToString());
}
