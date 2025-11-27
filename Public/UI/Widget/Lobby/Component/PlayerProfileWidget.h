// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "PlayerProfileWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UPlayerProfileWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	UImage* Img_CharacterIcon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_CharacterText;
};
