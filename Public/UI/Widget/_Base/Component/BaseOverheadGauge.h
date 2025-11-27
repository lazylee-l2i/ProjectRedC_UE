// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "BaseOverheadGauge.generated.h"

class UProgressBar;
class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBaseOverheadGauge : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetGaugeColor(FLinearColor Color);
	void SetGaugeValue(float NewValue, float NewMaxValue);

	void BindGameplayAttribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);

	void OnValueChanged(const FOnAttributeChangeData& Data);
	void OnMaxValueChanged(const FOnAttributeChangeData& Data);

private:
	UPROPERTY(VisibleAnywhere, Meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	float CachedValue;
	float CachedMaxValue;
};
