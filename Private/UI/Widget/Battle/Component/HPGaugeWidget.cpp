// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/Component/HPGaugeWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Types/RedCLogChannels.h"


void UHPGaugeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (TickDownHP != CurrentHP)
	{
		TickDownHP = UKismetMathLibrary::FInterpTo(TickDownHP,CurrentHP,InDeltaTime,2);
		GaugeMaterialInstance->SetScalarParameterValue(FName("TickDownValue"),TickDownHP);		
	}
}

void UHPGaugeWidget::SetCurrentValue(float Value)
{
	GaugeMaterialInstance->SetScalarParameterValue(FName("Value"),Value);
	CurrentHP=Value;
	//UE_LOG(LogRedCWidget, Warning, TEXT("Current HP Value Changed : %f"), CurrentHP);
}

