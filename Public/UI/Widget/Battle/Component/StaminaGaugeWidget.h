// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/_Base/Component/BaseGauge.h"
#include "StaminaGaugeWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UStaminaGaugeWidget : public UBaseGauge
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY()
	float CurrentStamina=0;
	UPROPERTY()
	float TickDownStamina=0;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentValue(float Value);
	
	
};
