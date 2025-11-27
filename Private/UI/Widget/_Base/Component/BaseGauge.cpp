// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/_Base/Component/BaseGauge.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Kismet/KismetMaterialLibrary.h"

// 편집기용 내부 실시간 확인용 함수
void UBaseGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (!Img_Gauge || !BaseGaugeMaterial) return;
	
	if (IsDesignTime())
	{
		if (UMaterialInstanceDynamic* PreviewMID = UMaterialInstanceDynamic::Create(BaseGaugeMaterial, this))
		{
			PreviewMID->SetVectorParameterValue(TEXT("BG_Color"),      BG_Color);
			PreviewMID->SetVectorParameterValue(TEXT("Stat_Current"),  Stat_CurrentColor);
			PreviewMID->SetVectorParameterValue(TEXT("Stat_Minus"),    Stat_MinusColor);

			Img_Gauge->SetBrushFromMaterial(PreviewMID);
		}
		return;
	}
}

void UBaseGauge::NativeConstruct()
{
	Super::NativeConstruct();

	GaugeMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(),BaseGaugeMaterial);
	Img_Gauge->SetBrushFromMaterial(GaugeMaterialInstance);
	GaugeMaterialInstance->SetVectorParameterValue(FName("BG_Color"), BG_Color);
	GaugeMaterialInstance->SetVectorParameterValue(FName("Stat_Current"), Stat_CurrentColor);
	GaugeMaterialInstance->SetVectorParameterValue(FName("Stat_Minus"), Stat_MinusColor);
}
