// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "BaseScreenFadeIn.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBaseScreenFadeIn : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UImage* Img_FadeIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnim;

public:
	void PlayFadeInAnim();
};
