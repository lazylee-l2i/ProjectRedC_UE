// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseInGameHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBaseInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnimOptional), Transient)
	UWidgetAnimation* EnterFadeInAnim;
};
