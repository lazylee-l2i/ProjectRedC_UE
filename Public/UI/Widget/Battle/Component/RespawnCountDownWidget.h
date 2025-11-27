// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "RespawnCountDownWidget.generated.h"

class UEditableTextBox;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API URespawnCountDownWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget), FieldNotify)
	UEditableTextBox* RespawnTime;

	UPROPERTY(meta=(BindWidget))
	UOverlay* Overlay_RespawnCount;
};
