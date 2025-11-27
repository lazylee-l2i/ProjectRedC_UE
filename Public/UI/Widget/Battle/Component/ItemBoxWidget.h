// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "UI/Widget/_Base/Component/BaseItemBoxImage.h"
#include "ItemBoxWidget.generated.h"

class UBattleViewModel;
class UShopItemBoxListEntryData;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UItemBoxWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

public:
	void StartCoolDownAnim(float CoolDown);
	UFUNCTION()
	void EndCoolDownAnim();
	void SetWidgetNextItem(int32 ItemLevel);
	void SetItemBoxHidden();

public:
	UPROPERTY()
	TObjectPtr<UShopItemBoxListEntryData> ItemData = nullptr;
	FWidgetAnimationDynamicEvent EndDelegate;
	float TotalCoolDown = 0;
	float CoolDownRemaining = 0;
	
	
public:
	// CoolDown
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* CoolDownAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* CoolDownFinished;

	UPROPERTY(meta=(BindWidget))
	UImage* Img_Cooldown;

	UPROPERTY(meta=(BindWidget))
	UImage* Img_GrayBG;

	UPROPERTY(meta=(BindWidget))
	UImage* Img_CooldownFinished; 
	
	// View
	UPROPERTY(meta=(BindWidget))
	UOverlay* Overlay_Total;
	
	UPROPERTY(meta=(BindWidget))
	UBaseItemBoxImage* Img_ItemInfo;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_ItemName;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_ItemPrice;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_ItemLevel;
};
