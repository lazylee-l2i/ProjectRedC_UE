// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 Priority = 0;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;
};

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	TPair<UInputMappingContext*, int32> FindInputMappingContextForTag(const FGameplayTag& ContextTag) const;
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag) const;

	// Warrior Ability에서 Player_Ability_Attack를 기반으로 뒤쪽 AttackType 가져오기

	
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping Context", Meta = (Categories = "Input.Context"))
	TMap<FGameplayTag, FInputMappingContextAndPriority> InputMappingContextMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input Action", Meta = (Categories = "Input.Action.Native"))
	TMap<FGameplayTag, TObjectPtr<UInputAction>> NativeActionMap;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action", Meta = (Categories = "Input.Action.Ability"))
	TMap<FGameplayTag, TObjectPtr<UInputAction>> AbilityActionMap;
};
