// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBoxWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "UI/MVVM/BattleViewModel.h"
#include "BattleShopWidget.generated.h"

class UWidgetSwitcher;
struct FRedCItemInfo;
class UWrapBox;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBattleShopWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetBattleViewModelBinding(UBattleViewModel* ViewModel);
	// 초기 아이템 리스트
	UFUNCTION(BlueprintCallable)
	void ItemListInit(const FRedCItemArray& CharacterItemInfo);
	
	void StartItemCoolTime(FName ItemID);
	void EndItemCoolTime(FName ItemID);
	
	// 변경된 아이템
	void FindAndChangeItemInfo(FName ItemID, int32 ItemLevel);

	// 현재 활성화된 Widget Switcher
	UListView* GetActiveListView();
	void BattleShopPageChange();

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* Text_PlayerGold;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* Text_PlayerLevel;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* Text_PlayerName;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* Switcher_Pages;
	
	UPROPERTY(EditDefaultsOnly, Category="ItemBox")
	TSubclassOf<UItemBoxWidget> ItemBoxClass;

	UPROPERTY(meta=(BindWidget))
	UListView* List_ItemSlot_1;

	UPROPERTY(meta=(BindWidget))
	UListView* List_ItemSlot_2;

public:
	int32 MaxItemSlot = 8;
	
};
