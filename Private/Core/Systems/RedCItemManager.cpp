// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/RedCItemManager.h"

#include "Core/Systems/RedCItemAsset.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "GAS/Attributes/RedCCurrencySet.h"
#include "Types/RedCLogChannels.h"


void URedCItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		static const TCHAR* Path = TEXT("/Game/Data/ItemInfo/DA_ItemList.DA_ItemList");
		URedCItemAsset* DA = Cast<URedCItemAsset>(StaticLoadObject(URedCItemAsset::StaticClass(), nullptr, Path));

		ItemCache(DA->HandItemDataTable, &CachedHandItem);
		ItemCache(DA->HatItemDataTable, &CachedHatItem);
		ItemCache(DA->ChestItemDataTable, &CachedChestItem);
		ItemCache(DA->BeltItemDataTable, &CachedBeltItem);
		ItemCache(DA->PantsItemDataTable, &CachedPantsItem);
		ItemCache(DA->ShoeItemDataTable, &CachedShoeItem);
		ItemCache(DA->NecklessItemDataTable, &CachedNecklessItem);
		ItemCache(DA->HealPotionItemDataTable, &CachedHealPotionItem);
		ItemCache(DA->BuffPotionItemDataTable, &CachedBuffPotionItem);
	}
}


void URedCItemManager::ItemCache(UDataTable* DataTable, TMap<FName, FRedCItemInfoTable>* Map)
{
	TArray<FName> RowNames = DataTable->GetRowNames();

	for (FName& Name : RowNames)
	{
		FRedCItemInfoTable* Row = DataTable->FindRow<FRedCItemInfoTable>(
		Name,
		FString::Printf(TEXT("HeadItemDataTable Error in Row: %s"), *Name.ToString())); //error log
        
		if (Row)
		{
			Map->Add(Name, *Row);
		}
	}
}

TArray<TPair<EItemType, FName>> URedCItemManager::GetHeroItemSet(FString CharacterID)
{
	if (CharacterID == "Character.Warrior")
	{
		return TArray{
			TPair<EItemType, FName>(EItemType::Hand,  "Item_Hand_01"),
			TPair<EItemType, FName>(EItemType::Hat,  "Item_Hat_01"),
			TPair<EItemType, FName>(EItemType::Chest,  "Item_Chest_01"),
			TPair<EItemType, FName>(EItemType::Belt,  "Item_Belt_01"),
			TPair<EItemType, FName>(EItemType::Pants,  "Item_Pants_01"),
			TPair<EItemType, FName>(EItemType::Shoe, "Item_Shoe_01"),
			TPair<EItemType, FName>(EItemType::Neckless, "Item_Neckless_01"),
			TPair<EItemType, FName>(EItemType::HealPotion, "Item_HealPotion_01"),
			TPair<EItemType, FName>(EItemType::AttackBuffPotion, "Item_AttackPotion_01"),
			TPair<EItemType, FName>(EItemType::DefenseBuffPotion, "Item_DefensePotion_01"),
		};
	}
	else if (CharacterID == "Character.Archer")
	{
		return TArray{
			TPair<EItemType, FName>(EItemType::Hand,  "Item_Hand_02"),
			TPair<EItemType, FName>(EItemType::Hat,  "Item_Hat_02"),
			TPair<EItemType, FName>(EItemType::Chest,  "Item_Chest_02"),
			TPair<EItemType, FName>(EItemType::Belt,  "Item_Belt_02"),
			TPair<EItemType, FName>(EItemType::Pants,  "Item_Pants_02"),
			TPair<EItemType, FName>(EItemType::Shoe, "Item_Shoe_02"),
			TPair<EItemType, FName>(EItemType::Neckless, "Item_Neckless_02"),
			TPair<EItemType, FName>(EItemType::HealPotion, "Item_HealPotion_01"),
			TPair<EItemType, FName>(EItemType::AttackBuffPotion, "Item_AttackPotion_01"),
			TPair<EItemType, FName>(EItemType::DefenseBuffPotion, "Item_DefensePotion_01"),
		};
	}
	UE_LOG(LogRedCPlayer, Log, TEXT("No Item Available"));
	return {};
}

bool URedCItemManager::bItemTypeIsHealPotion(FName ItemID)
{
	if (ItemID == NAME_None) return false;

	for (auto k : CachedHealPotionItem)
	{
		if (k.Value.ItemInfo.ItemID == ItemID)
		{
			return true;	
		}
	}
	
	return false;
}

bool URedCItemManager::bItemTypeIsBuffPotion(FName ItemID)
{
	if (ItemID == NAME_None) return false;

	for (auto k : CachedBuffPotionItem)
	{
		if (k.Value.ItemInfo.ItemID == ItemID)
		{
			return true;	
		}
	}
	
	return false;
}


int32 URedCItemManager::GetItemSlotArray(EItemType ItemType)
{
	switch (ItemType)
	{
	case EItemType::Hand:
		{
			return 0;
			break;
		}
		
	case EItemType::Hat:
		{
			return 1;
			break;
		}

	case EItemType::Chest:
		{
			return 2;
			break;
		}

	case EItemType::Belt:
		{
			return 3;
			break;
		}

	case EItemType::Pants:
		{
			return 4;
			break;
		}

	case EItemType::Shoe:
		{
			return 5;
			break;
		}

	case EItemType::Neckless:
		{
			return 6;
			break;
		}

	case EItemType::HealPotion:
		{
			return 8;
			break;
		}

	case EItemType::AttackBuffPotion:
		{
			return 9;
			break;
		}
		
	case EItemType::DefenseBuffPotion:
		{
			return 10;
			break;
		}
		
	default:
		{
			UE_LOG(LogRedCWidget, Warning, TEXT("Setting Item Null #1"));
			return -1;
			break;
		}
	};
}



//server
// DT 정보 추출 후 RedCItemInfo 타입변환
FRedCItemInfo URedCItemManager::ParseDataTableToItemInfo(const FName& ItemRowName, FRedCItemInfoTable* FindItem)
{
	FRedCItemInfo ItemInfo;
	ItemInfo.ItemID = FindItem->ItemInfo.ItemID;
	ItemInfo.ItemLevel = FindItem->ItemInfo.ItemLevel;
	ItemInfo.ItemMaxLevel = FindItem->ItemInfo.ItemMaxLevel;
	ItemInfo.ItemCoolTime = FindItem->ItemInfo.ItemCoolTime;
	ItemInfo.ItemIcon = FindItem->ItemInfo.ItemIcon;
	ItemInfo.ItemName = FindItem->ItemInfo.ItemName;
	ItemInfo.ItemProperty = FindItem->ItemInfo.ItemProperty;
	
	return ItemInfo;
}

//server
FRedCItemInfoTable* URedCItemManager::FindItem(const EItemType& ItemType, const FName& ItemRowName)
{
	FRedCItemInfoTable* FoundItemInfo = nullptr;
	switch (ItemType)
	{
		case EItemType::Hand:
			FoundItemInfo=CachedHandItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::Hat:
			FoundItemInfo=CachedHatItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::Chest:
			FoundItemInfo=CachedChestItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::Belt:
			FoundItemInfo=CachedBeltItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::Pants:
			FoundItemInfo=CachedPantsItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::Shoe:
			FoundItemInfo=CachedShoeItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::Neckless:
			FoundItemInfo=CachedNecklessItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::HealPotion:
			FoundItemInfo=CachedHealPotionItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::AttackBuffPotion:
			FoundItemInfo=CachedBuffPotionItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
		case EItemType::DefenseBuffPotion:
			FoundItemInfo=CachedBuffPotionItem.Find(ItemRowName);
			return FoundItemInfo;
			break;
	default:
			UE_LOG(LogRedC, Warning, TEXT("Cant Find Item Data - ItemType"));
			break;
	}

	if (!FoundItemInfo)
	{
		UE_LOG(LogRedC, Warning, TEXT("Cant Find Item Data - ItemInfo"));
		return nullptr;
	}
	
	return FoundItemInfo;
}

