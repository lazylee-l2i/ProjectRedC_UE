// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Button/CharacterSelectButtonWidget.h"
#include "CharacterSelectBox.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UCharacterSelectBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UCharacterSelectButtonWidget* Btn_Warrior;
	UPROPERTY(meta=(BindWidget))
	UCharacterSelectButtonWidget* Btn_Archor;
	//UPROPERTY(meta=(BindWidget))
	//UCharacterSelectButtonWidget* Btn_Magician;
};
