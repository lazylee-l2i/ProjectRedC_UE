// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/Component/ItemBoxWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Types/RedCLogChannels.h"
#include "UI/MVVM/BattleViewModel.h"
#include "UI/Type/ShopItemBoxListEntryData.h"


void UItemBoxWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	if (UShopItemBoxListEntryData* Data = Cast<UShopItemBoxListEntryData>(ListItemObject))
	{
		// 빈값일 경우 Hidden 하고 리턴
		if (Data->ItemInfo.ItemID.IsNone())
		{
			Overlay_Total->SetVisibility(ESlateVisibility::Hidden);
			return;
		}

		Img_ItemInfo->Text_BindingKey->SetText(FText::AsNumber( Data->InputKeyText));

		// 캐싱
		ItemData = Data;
		
		if (Img_ItemInfo)  { Img_ItemInfo->Img_Item->SetBrushFromSoftTexture(Data->ItemInfo.ItemIcon); }
		if (Text_ItemName) { Text_ItemName->SetText(FText::FromString( Data->ItemInfo.ItemName)); }
		if (Text_ItemPrice) { Text_ItemPrice->SetText(FText::AsNumber(Data->ItemInfo.ItemProperty[Data->ItemInfo.ItemLevel].ItemPrice)); }
		if (Text_ItemLevel) { Text_ItemLevel->SetText(FText::AsNumber(Data->ItemInfo.ItemLevel+1)); }
	}
}


void UItemBoxWidget::StartCoolDownAnim(float CoolDown)
{
	float AnimLen = CoolDownAnim->GetEndTime();
	float PlayRate = AnimLen/CoolDown;

	Img_GrayBG->SetVisibility(ESlateVisibility::Visible);
	Img_Cooldown->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(CoolDownAnim, 0, 1, EUMGSequencePlayMode::Forward, PlayRate);
	
	EndDelegate.Clear();
	EndDelegate.BindDynamic(this, &ThisClass::EndCoolDownAnim);
	BindToAnimationFinished(CoolDownAnim, EndDelegate);
}

void UItemBoxWidget::EndCoolDownAnim()
{
	Img_GrayBG->SetVisibility(ESlateVisibility::Hidden);
	Img_Cooldown->SetVisibility(ESlateVisibility::Hidden);
	
	PlayAnimation(CoolDownFinished, 0, 1, EUMGSequencePlayMode::Forward);
}


void UItemBoxWidget::SetWidgetNextItem(int32 ItemLevel)
{
	// 최대레벨일 경우 리턴
	if(ItemLevel>ItemData->ItemInfo.ItemMaxLevel) return;

	if (Text_ItemLevel) { Text_ItemLevel->SetText(FText::AsNumber(ItemLevel+1)); }
	if (Text_ItemPrice) { Text_ItemPrice->SetText(FText::AsNumber(ItemData->ItemInfo.ItemProperty[ItemLevel].ItemPrice)); }
	//UE_LOG(LogRedCWidget, Warning,TEXT("Item Changed ItemID : %s, ItemLevel : %d"), *ItemData->ItemInfo.ItemName, ItemLevel);
}

void UItemBoxWidget::SetItemBoxHidden()
{
	Overlay_Total->SetVisibility(ESlateVisibility::Hidden);
}
