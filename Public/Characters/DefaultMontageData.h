// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DefaultMontageData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UDefaultMontageData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> MontageMap;
	
};
