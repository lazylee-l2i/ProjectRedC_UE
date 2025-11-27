// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "RedCAssetData.generated.h"

USTRUCT()
struct FAssetEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FName AssetName;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Asset"))
	FGameplayTag AssetTag;

	UPROPERTY(EditDefaultsOnly)
	FSoftObjectPath AssetPath;

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> AssetLabels;
};

USTRUCT()
struct FAssetSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FAssetEntry> AssetEntries;
};

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCAssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FSoftObjectPath GetAssetPathByName(const FName& Name);
	FSoftObjectPath GetAssetPathByTag(const FGameplayTag& Tag);
	
	const FAssetSet& GetAssetByLabel(const FName& Label);

	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FAssetSet> AssetGroupNameToSet;

	UPROPERTY()
	TMap<FName, FSoftObjectPath> AssetNameToPath;

	UPROPERTY()
	TMap<FGameplayTag, FSoftObjectPath> AssetTagToPath;

	UPROPERTY()
	TMap<FName, FAssetSet> AssetLabelToSet;
};
