// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RedCItemAsset.generated.h"

struct FRedCItemInfoTable;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCItemAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	UDataTable* HandItemDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* HatItemDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* ChestItemDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* BeltItemDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* PantsItemDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* ShoeItemDataTable = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	UDataTable* NecklessItemDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* HealPotionItemDataTable = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* BuffPotionItemDataTable = nullptr;
};
