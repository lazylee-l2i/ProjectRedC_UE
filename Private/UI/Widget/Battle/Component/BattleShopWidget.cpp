// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/Component/BattleShopWidget.h"

#include <functional>

#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"
#include "Types/RedCLogChannels.h"
#include "UI/Type/ShopItemBoxListEntryData.h"


void UBattleShopWidget::SetBattleViewModelBinding(UBattleViewModel* ViewModel)
{
	if (ViewModel)
	{
		// 아이템 레벨 델리게이트 바인딩
		ViewModel->OnChangedItemLevel.AddUObject(this, &ThisClass::FindAndChangeItemInfo);
		// 아이템 쿨다임 델리게이트 바인딩
		ViewModel->OnStartedItemCoolTime.AddUObject(this, &ThisClass::StartItemCoolTime);
		ViewModel->OnEndItemCoolTime.AddUObject(this, &ThisClass::EndItemCoolTime);
	}
	else
	{
		UE_LOG(LogRedCWidget, Warning,TEXT("No BattleViewModel"));
	}
}


void UBattleShopWidget::ItemListInit(const FRedCItemArray& CharacterItemInfo)
{
	if (List_ItemSlot_1) List_ItemSlot_1->ClearListItems();
	if (List_ItemSlot_2) List_ItemSlot_2->ClearListItems();

	int32 Index = 0;
	for (const FRedCItemInfo& Item : CharacterItemInfo.Items)
	{
		UShopItemBoxListEntryData* NewItemData = NewObject<UShopItemBoxListEntryData>(this, UShopItemBoxListEntryData::StaticClass());
		
		if (Index < MaxItemSlot) // 앞 8개
		{
			if (List_ItemSlot_1)
			{
				NewItemData->ItemInfo = Item;
				NewItemData->InputKeyText = Index+1;
				List_ItemSlot_1->AddItem(NewItemData);
			}
		}
		else // 나머지
		{
			if (List_ItemSlot_2)
			{
				NewItemData->ItemInfo = Item;
				NewItemData->InputKeyText = Index+1-8;
				List_ItemSlot_2->AddItem(NewItemData);
			}
		}
		++Index;
	}
}

void UBattleShopWidget::StartItemCoolTime(FName ItemID)
{
	UListView* ActiveListView = GetActiveListView();
	for(auto ListItems : ActiveListView->GetListItems())
	{
		if (UShopItemBoxListEntryData* Data = Cast<UShopItemBoxListEntryData>(ListItems))
		{
			if (Data->ItemInfo.ItemID == ItemID)
			{
				if (UUserWidget* Entry = ActiveListView->GetEntryWidgetFromItem(Data))
				{
					if (UItemBoxWidget* Row = Cast<UItemBoxWidget>(Entry))
					{
						Row->StartCoolDownAnim(Data->ItemInfo.ItemCoolTime);
					}
				}
				break;
			}
		}
	}
}

void UBattleShopWidget::EndItemCoolTime(FName ItemID)
{
	UListView* ActiveListView = GetActiveListView();
	for(auto ListItems : ActiveListView->GetListItems())
	{
		if (UShopItemBoxListEntryData* Data = Cast<UShopItemBoxListEntryData>(ListItems))
		{
			if (Data->ItemInfo.ItemID == ItemID)
			{
				if (UUserWidget* Entry = ActiveListView->GetEntryWidgetFromItem(Data))
				{
					if (UItemBoxWidget* Row = Cast<UItemBoxWidget>(Entry))
					{
						// 애니메이션으로 처리하므로 End는 잠시보류
						//Row->EndCoolDownAnim();
					}
				}
				break;
			}
		}
	}
}



void UBattleShopWidget::FindAndChangeItemInfo(FName ItemID, int32 ItemLevel)
{
	// 여기서 아이템 바뀐정보 받으면 수정
	// mvvm에서는 바뀐정보내려오는것.
	UE_LOG(LogRedCWidget, Warning, TEXT("Item Changed ItemID : %s, ItemLevel : %d"), *ItemID.ToString(), ItemLevel);

	// 전체 List_ItemSlot 중 ItemID로 찾기 -> 해당 EntryWidget Refresh
	UListView* ActiveListView = GetActiveListView();
	for(auto ListItems : ActiveListView->GetListItems())
	{
		if (UShopItemBoxListEntryData* Data = Cast<UShopItemBoxListEntryData>(ListItems))
		{
			if (Data->ItemInfo.ItemID == ItemID)
			{
				if (UUserWidget* Entry = ActiveListView->GetEntryWidgetFromItem(Data))
				{
					if (UItemBoxWidget* Row = Cast<UItemBoxWidget>(Entry))
					{
						if (ItemLevel == Row->ItemData->ItemInfo.ItemMaxLevel)
						{
							Row->SetItemBoxHidden();
							break;
						}
						Row->SetWidgetNextItem(ItemLevel);
					}
				}
				break;
			}
		}
	}

	
}

UListView* UBattleShopWidget::GetActiveListView()
{
	if (!Switcher_Pages)
	{
		UE_LOG(LogRedCWidget, Error, TEXT("Switcher_Pages is null"));
		return nullptr;
	}

		const int32 Idx = Switcher_Pages->GetActiveWidgetIndex();
		if (Idx == 0 && List_ItemSlot_1) return List_ItemSlot_1;
		if (Idx == 1 && List_ItemSlot_2) return List_ItemSlot_2;

		UE_LOG(LogRedCWidget, Warning, TEXT("Active widget is not a ListView"));

	return nullptr;
}

void UBattleShopWidget::BattleShopPageChange()
{
	int32 Current = Switcher_Pages->GetActiveWidgetIndex();
	int32 Next = (Current == 0) ? 1 : 0;
	Switcher_Pages->SetActiveWidgetIndex(Next);
}
