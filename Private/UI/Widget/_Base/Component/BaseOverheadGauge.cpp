// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/_Base/Component/BaseOverheadGauge.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"


void UBaseOverheadGauge::SetGaugeColor(FLinearColor Color)
{
	if (ProgressBar)
	{
		ProgressBar->SetFillColorAndOpacity(Color);
	}
}

void UBaseOverheadGauge::SetGaugeValue(float NewValue, float NewMaxValue)
{
	CachedValue = NewValue;
	CachedMaxValue = NewMaxValue;

	if (ProgressBar && NewMaxValue != 0)
	{
		ProgressBar->SetPercent(NewValue / NewMaxValue);
	}
}

void UBaseOverheadGauge::BindGameplayAttribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
	if (ASC)
	{
		bool bFound;
		const float Value = ASC->GetGameplayAttributeValue(Attribute, bFound);
		const float MaxValue = ASC->GetGameplayAttributeValue(MaxAttribute, bFound);

		if (bFound)
		{
			SetGaugeValue(Value, MaxValue);
		}
		
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::OnValueChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &ThisClass::OnMaxValueChanged);
	}
}

void UBaseOverheadGauge::OnValueChanged(const FOnAttributeChangeData& Data)
{
	SetGaugeValue(Data.NewValue, CachedMaxValue);
}

void UBaseOverheadGauge::OnMaxValueChanged(const FOnAttributeChangeData& Data)
{
	SetGaugeValue(CachedValue, Data.NewValue);
}
