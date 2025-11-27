// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/_Base/Component/BaseSkillBox.h"
#include "TwoKeySkillBox.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UTwoKeySkillBox : public UBaseSkillBox
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
public:
	// 위젯 Visible 옵션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	bool BlackBG_Visible = true;
	
public:
	// Mouse Icon 용 옵션
	UPROPERTY(meta=(BindWidget))
	UImage* Img_Icon_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FSlateBrush IconBrush_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FVector2D IconSize_2 = FVector2D(64.f, 64.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWidgetTransform IconRenderTransform_2;
	UPROPERTY(meta=(BindWidget))
	UImage* Img_BlackBG;
	
};
