// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchingButtonWidget.h"
#include "MVVMViewModelBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MatchingWidget.generated.h"

class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UMatchingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
public:
	UFUNCTION()
	void OnMatchClicked();

	
public:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcher_Matching;

	UPROPERTY(meta=(BindWidget))
	UMatchingButtonWidget* Button_Matching;

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerTarget = 0;
};
