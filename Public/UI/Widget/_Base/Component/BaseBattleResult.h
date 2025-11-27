// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "BaseBattleResult.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBaseBattleResult : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* MoveDownAnim;
	
public:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcher_Result;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* Text_Result_Win;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Result_Lose;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Result_Draw;
	void PlayMoveDownAnim();
};
