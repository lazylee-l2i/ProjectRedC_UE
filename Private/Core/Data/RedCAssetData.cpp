// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Data/RedCAssetData.h"

#include "Types/RedCLogChannels.h"
#include "UObject/ObjectSaveContext.h"


FSoftObjectPath URedCAssetData::GetAssetPathByName(const FName& Name)
{
	FSoftObjectPath* AssetPath = AssetNameToPath.Find(Name);

	ensureAlwaysMsgf(AssetPath, TEXT("Can not find Asset Path from Asset Name [%s]."), *Name.ToString());

	return *AssetPath;
}

FSoftObjectPath URedCAssetData::GetAssetPathByTag(const FGameplayTag& Tag)
{
	FSoftObjectPath* AssetPath = AssetTagToPath.Find(Tag);

	ensureAlwaysMsgf(AssetPath, TEXT("Can not find Asset Path from Asset Tag [%s]."), *Tag.ToString());

	return *AssetPath;
}

const FAssetSet& URedCAssetData::GetAssetByLabel(const FName& Label)
{
	const FAssetSet* AssetSet = AssetLabelToSet.Find(Label);

	ensureAlwaysMsgf(AssetSet, TEXT("Can not find Asset Set from Asset Label [%s]."), *Label.ToString());

	return *AssetSet;
}

void URedCAssetData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	AssetNameToPath.Empty();
	AssetTagToPath.Empty();
	AssetLabelToSet.Empty();

	AssetGroupNameToSet.KeySort([](const FName& A, const FName& B)
	{
		return A.Compare(B) < 0;
	});

	for (const auto& Pair : AssetGroupNameToSet)
	{
		const FAssetSet& AssetSet = Pair.Value;
		for (FAssetEntry AssetEntry : AssetSet.AssetEntries)
		{
			FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
			
			const FString& AssetName = AssetPath.GetAssetName();
			if (AssetName.StartsWith(TEXT("BP_")) || AssetName.StartsWith(TEXT("ABP_")) ||
				AssetName.StartsWith(TEXT("GE_")) || AssetName.StartsWith(TEXT("GA_")))
			{
				FString AssetPathString = AssetPath.GetAssetPathString();
				AssetPathString.Append(TEXT("_C"));
				AssetPath = FSoftObjectPath(AssetPathString);
			}
			
			AssetNameToPath.Emplace(AssetEntry.AssetName, AssetEntry.AssetPath);
			AssetTagToPath.Emplace(AssetEntry.AssetTag, AssetEntry.AssetPath);
			for (const FName& Label : AssetEntry.AssetLabels)
			{
				AssetLabelToSet.FindOrAdd(Label).AssetEntries.Emplace(AssetEntry);
			}
		}
	}

	UE_LOG(LogRedC, Warning, TEXT("PreSave %s"), SaveContext.GetTargetFilename());
}
