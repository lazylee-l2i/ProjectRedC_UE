// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "UI/Widget/_Base/Component/BaseCountDown.h"
#include "GamePlayCountDownWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UGamePlayCountDownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget), FieldNotify)
	UEditableTextBox* RemainGamePlayTime;
};
