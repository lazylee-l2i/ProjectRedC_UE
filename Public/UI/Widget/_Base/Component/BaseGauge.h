// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "BaseGauge.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBaseGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* GaugeMaterialInstance;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Material")
	UMaterialInterface* BaseGaugeMaterial;

public:
	UPROPERTY(meta=(BindWidget))
	UImage* Img_Gauge;
	UPROPERTY(EditAnywhere, Category="Material")
	FLinearColor BG_Color;
	UPROPERTY(EditAnywhere, Category="Material")
	FLinearColor Stat_CurrentColor;
	UPROPERTY(EditAnywhere, Category="Material")
	FLinearColor Stat_MinusColor;

protected:
	float CurrentValue = 0.f;
	float TickDownValue = 0.f;
	
};
