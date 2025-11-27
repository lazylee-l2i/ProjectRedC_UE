// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/Component/StaminaGaugeWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Types/RedCLogChannels.h"


void UStaminaGaugeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (TickDownStamina != CurrentStamina)
	{
		TickDownStamina = UKismetMathLibrary::FInterpTo(TickDownStamina,CurrentStamina,InDeltaTime,2);
		GaugeMaterialInstance->SetScalarParameterValue(FName("TickDownValue"),TickDownStamina);		
	}
}

void UStaminaGaugeWidget::SetCurrentValue(float Value)
{
	GaugeMaterialInstance->SetScalarParameterValue(FName("Value"),Value);
	CurrentStamina=Value;
	// UE_LOG(LogRedCWidget, Warning, TEXT("Current Stamina Value Changed : %f"), CurrentStamina);
}
