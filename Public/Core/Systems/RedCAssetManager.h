// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/Data/RedCAssetData.h"
#include "Engine/AssetManager.h"
#include "Types/RedCLogChannels.h"
#include "RedCAssetManager.generated.h"

DECLARE_DELEGATE_TwoParams(FAsyncLoadCompletedDelegate, const FName& /* Asset Name or Label */, UObject* /* Loaded Asset */);

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	URedCAssetManager();
	static URedCAssetManager& Get();

	static void Initialize();
	static void DeInitialize();
	
	static void LoadSyncByPath(const FSoftObjectPath& AssetPath);
	static void LoadSyncByName(const FName& AssetName);
	static void LoadSyncByLabel(const FName& Label);

	static void LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompleteDelegate = FAsyncLoadCompletedDelegate());
	static void LoadAsyncByName(const FName& AssetName, FAsyncLoadCompletedDelegate CompleteDelegate = FAsyncLoadCompletedDelegate());

	static void ReleaseByPath(const FSoftObjectPath& AssetPath);
	static void ReleaseByName(const FName& AssetName);
	static void ReleaseByLabel(const FName& Label);
	static void ReleaseAll();
	
private:
	void LoadPreloadAssets(const FPrimaryAssetType& PrimaryAssetType);
	void AddLoadedAsset(const FName& AssetName, const UObject* Asset);
	
	UPROPERTY()
	TObjectPtr<URedCAssetData> LoadedAssetData;

	UPROPERTY()
	TMap<FName, TObjectPtr<const UObject>> NameToLoadedAsset;

public:
	template<typename AssetType>
	static AssetType* GetAssetByName(const FName& AssetName);

	template<typename AssetType>
	static AssetType* GetAssetByTag(const FGameplayTag& AssetTag);
};

template <typename AssetType>
AssetType* URedCAssetManager::GetAssetByName(const FName& AssetName)
{
	URedCAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	if (AssetPath.IsValid())
	{
		LoadedAsset = Cast<AssetType>(AssetPath.ResolveObject());
		if (LoadedAsset == nullptr)
		{
			UE_LOG(LogRedC, Warning, TEXT("Attempted sync loading because asset had not loaded yet [%s]."), *AssetPath.ToString());
			LoadedAsset = Cast<AssetType>(AssetPath.TryLoad());
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
AssetType* URedCAssetManager::GetAssetByTag(const FGameplayTag& AssetTag)
{
	URedCAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByTag(AssetTag);
	if (AssetPath.IsValid())
	{
		LoadedAsset = Cast<AssetType>(AssetPath.ResolveObject());
		if (LoadedAsset == nullptr)
		{
			UE_LOG(LogRedC, Warning, TEXT("Attempted sync loading because asset had not loaded yet [%s]."), *AssetPath.ToString());
			LoadedAsset = Cast<AssetType>(AssetPath.TryLoad());
		}
	}

	return LoadedAsset;
}
