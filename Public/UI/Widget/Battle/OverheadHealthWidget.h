// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Battle/OverheadWidget.h"
#include "OverheadHealthWidget.generated.h"

class UTextBlock;
class UBaseOverheadGauge;
class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UOverheadHealthWidget : public UOverheadWidget
{
	GENERATED_BODY()

public:
	void BindHealthAttribute(UAbilitySystemComponent* ASC);
	
	virtual void SetTeamColor(bool bIsSameTeam) override;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UBaseOverheadGauge> HealthGauge;

	// 0 : Enemy, 1 : Friend
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Gauge Color")
	TArray<FLinearColor> TeamColor;
};
