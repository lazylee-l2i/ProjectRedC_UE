// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "UI/Widget/_Base/Component/BaseSkillBox.h"
#include "OneKeySkillBox.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UOneKeySkillBox : public UBaseSkillBox
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	// 위젯 Visible 옵션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	bool WhiteBG_Visible = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	bool BlackBG_Visible = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	bool Border_CheckBox_Visible = true;
	
public:
	UPROPERTY(meta=(BindWidget))
	UImage* Img_WhiteBG;
	UPROPERTY(meta=(BindWidget))
	UImage* Img_BlackBG;
	UPROPERTY(meta=(BindWidget))
	UImage* Img_Border;
	
	
};
