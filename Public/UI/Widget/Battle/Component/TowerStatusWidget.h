// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "TowerStatusWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UTowerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget), FieldNotify)
	UWidgetSwitcher* WidgetSwitcher_Tower1;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget), FieldNotify)
	UWidgetSwitcher* WidgetSwitcher_Tower2;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	UImage* Img_Tower_1;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	UImage* Img_BrokenTower_1;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	UImage* Img_Tower_2;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	UImage* Img_BrokenTower_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Icons")
	FSlateBrush Icon_Tower_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Icons")
	FSlateBrush Icon_BrokenTower_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Icons")
	FSlateBrush Icon_Tower_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Icons")
	FSlateBrush Icon_BrokenTower_2;

public:
	UFUNCTION(BlueprintCallable)
	void SetTowerCount(int InputTowerCount);
	void TowerIconChange();
	
	UPROPERTY(BlueprintReadWrite)
	int TowerCount = 0;
};
