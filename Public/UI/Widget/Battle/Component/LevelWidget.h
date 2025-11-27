// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LevelWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ULevelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* Text_PlayerLevel;
};
