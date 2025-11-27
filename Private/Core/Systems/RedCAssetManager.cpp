// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/RedCAssetManager.h"


URedCAssetManager::URedCAssetManager()
{
	
}

URedCAssetManager& URedCAssetManager::Get()
{
	if (URedCAssetManager* AssetManager = Cast<URedCAssetManager>(GEngine->AssetManager))
	{
		return *AssetManager;
	}

	return *NewObject<URedCAssetManager>();
}

void URedCAssetManager::Initialize()
{
	Get().LoadPreloadAssets(FPrimaryAssetType(FName("RedCAssetData")));
}

void URedCAssetManager::DeInitialize()
{
	Get().ReleaseAll();
}

void URedCAssetManager::LoadSyncByPath(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		UObject* LoadedAsset = AssetPath.ResolveObject();
		if (LoadedAsset == nullptr)
		{
			if (IsInitialized())
			{
				LoadedAsset = GetStreamableManager().LoadSynchronous(AssetPath, false);
			}
			else
			{
				LoadedAsset = AssetPath.TryLoad();
			}
		}

		if (LoadedAsset)
		{
			Get().AddLoadedAsset(AssetPath.GetAssetFName(), LoadedAsset);
		}
		else
		{
			UE_LOG(LogRedC, Fatal, TEXT("Failed to load asset[%s]"), *AssetPath.ToString());
		}
	}
}

void URedCAssetManager::LoadSyncByName(const FName& AssetName)
{
	URedCAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	LoadSyncByPath(AssetPath);
}

void URedCAssetManager::LoadSyncByLabel(const FName& Label)
{
	if (IsInitialized() == false)
	{
		UE_LOG(LogRedC, Error, TEXT("Asset Manager must be initialized"));
		return;
	}

	URedCAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	TArray<FSoftObjectPath> AssetPaths;

	const FAssetSet& AssetSet = AssetData->GetAssetByLabel(Label);
	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		LoadSyncByPath(AssetPath);
		if (AssetPath.IsValid())
		{
			AssetPaths.Emplace(AssetPath);
		}
	}

	GetStreamableManager().RequestSyncLoad(AssetPaths);

	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		if (AssetPath.IsValid())
		{
			if (UObject* LoadedAsset = AssetPath.ResolveObject())
			{
				Get().AddLoadedAsset(AssetEntry.AssetName, LoadedAsset);
			}
			else
			{
				UE_LOG(LogRedC, Fatal, TEXT("Failed to load asset [%s]."), *AssetPath.ToString());
			}
		}
	}
}

void URedCAssetManager::LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompleteDelegate)
{
	if (IsInitialized() == false)
	{
		UE_LOG(LogRedC, Error, TEXT("AssetManager must be initialized"));
		return;
	}

	if (AssetPath.IsValid())
	{
		if (UObject* LoadedAsset = AssetPath.ResolveObject())
		{
			Get().AddLoadedAsset(AssetPath.GetAssetFName(), LoadedAsset);
		}
		else
		{
			TArray<FSoftObjectPath> AssetPaths;
			AssetPaths.Add(AssetPath);

			TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(AssetPaths);
			Handle->BindCompleteDelegate(FStreamableDelegate::CreateLambda([AssetName = AssetPath.GetAssetFName(), AssetPath, CompleteDelegate = MoveTemp(CompleteDelegate)]()
			{
				UObject* LoadedAsset = AssetPath.ResolveObject();
				Get().AddLoadedAsset(AssetName, LoadedAsset);
				if (CompleteDelegate.IsBound())
				{
					CompleteDelegate.Execute(AssetName, LoadedAsset);
				}
			}));
		}
	}
}

void URedCAssetManager::LoadAsyncByName(const FName& AssetName, FAsyncLoadCompletedDelegate CompleteDelegate)
{
	if (IsInitialized() == false)
	{
		UE_LOG(LogRedC, Error, TEXT("AssetManager must be initialized"));
		return;
	}

	URedCAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	LoadAsyncByPath(AssetPath, CompleteDelegate);
}

void URedCAssetManager::ReleaseByPath(const FSoftObjectPath& AssetPath)
{
	FName AssetName = AssetPath.GetAssetFName();
	ReleaseByName(AssetName);
}

void URedCAssetManager::ReleaseByName(const FName& AssetName)
{
	URedCAssetManager& AssetManager = Get();
	if (AssetManager.NameToLoadedAsset.Contains(AssetName))
	{
		AssetManager.NameToLoadedAsset.Remove(AssetName);
	}
	else
	{
		UE_LOG(LogRedC, Log, TEXT("Can not find loaded asset by assetName [%s]."), *AssetName.ToString());
	}
}

void URedCAssetManager::ReleaseByLabel(const FName& Label)
{
	URedCAssetManager& AssetManager = Get();
	URedCAssetData* LoadedAssetData = AssetManager.LoadedAssetData;
	const FAssetSet& AssetSet = LoadedAssetData->GetAssetByLabel(Label);

	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FName& AssetName = AssetEntry.AssetName;
		if (AssetManager.NameToLoadedAsset.Contains(AssetName))
		{
			AssetManager.NameToLoadedAsset.Remove(AssetName);
		}
		else
		{
			UE_LOG(LogRedC, Log, TEXT("Can not find loaded asset by assetName [%s]."), *AssetName.ToString());
		}
	}
}

void URedCAssetManager::ReleaseAll()
{
	URedCAssetManager& AssetManager = Get();
	AssetManager.NameToLoadedAsset.Reset();
}

void URedCAssetManager::LoadPreloadAssets(const FPrimaryAssetType& PrimaryAssetType)
{
	if (LoadedAssetData)
		return;

	URedCAssetData* AssetData = nullptr;
	//FPrimaryAssetType PrimaryAssetType(URedCAssetData::StaticClass()->GetFName());
	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete(0.0f, false);
		AssetData = Cast<URedCAssetData>(Handle->GetLoadedAsset());
	}

	if (AssetData)
	{
		LoadedAssetData = AssetData;
		LoadSyncByLabel("Preload");
	}
	else
	{
		UE_LOG(LogRedC, Fatal, TEXT("Failed to load AssetData asset type [%s]."), *PrimaryAssetType.ToString());
	}
}

void URedCAssetManager::AddLoadedAsset(const FName& AssetName, const UObject* Asset)
{
	if (AssetName.IsValid() && Asset)
	{
		if (NameToLoadedAsset.Contains(AssetName) == false)
		{
			NameToLoadedAsset.Add(AssetName, Asset);
		}
	}

}
