// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "RedCItemAsset.h"
#include "Controllers/RedCPlayerController.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/RedCItemInfoTable.h"
#include "RedCItemManager.generated.h"

/**
 * 
 */
// todo - 나중에 BattleServer만 해당클래스 포함되도록 바꿔야함
// 현재 로비서버도 해당클래스 들고있음

UCLASS()
class PROJECTREDC_API URedCItemManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	TArray<TPair<EItemType, FName>> GetHeroItemSet(FString CharacterID);
	bool bItemTypeIsHealPotion(FName ItemID);
	bool bItemTypeIsBuffPotion(FName ItemID);

public:
	TMap<FName, FRedCItemInfoTable> CachedHandItem;
	TMap<FName, FRedCItemInfoTable> CachedHatItem;
	TMap<FName, FRedCItemInfoTable> CachedChestItem;
	TMap<FName, FRedCItemInfoTable> CachedBeltItem;
	TMap<FName, FRedCItemInfoTable> CachedPantsItem;
	TMap<FName, FRedCItemInfoTable> CachedShoeItem;
	TMap<FName, FRedCItemInfoTable> CachedNecklessItem;
	TMap<FName, FRedCItemInfoTable> CachedHealPotionItem;
	TMap<FName, FRedCItemInfoTable> CachedBuffPotionItem;

public:
	int32 GetItemSlotArray(EItemType ItemType);
	
public:
	void ItemCache(UDataTable* DataTable, TMap<FName, FRedCItemInfoTable>* Map);
	
	FRedCItemInfo ParseDataTableToItemInfo( const FName& ItemRowName, FRedCItemInfoTable* FindItem);
	FRedCItemInfoTable* FindItem(const EItemType& ItemType, const FName& ItemRowName);
	
};
