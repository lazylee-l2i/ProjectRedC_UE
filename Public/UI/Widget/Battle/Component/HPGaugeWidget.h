// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UI/Widget/_Base/Component/BaseGauge.h"
#include "HPGaugeWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UHPGaugeWidget : public UBaseGauge
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY()
	float CurrentHP=0;
	UPROPERTY()
	float TickDownHP=0;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentValue(float Value);
	
};
