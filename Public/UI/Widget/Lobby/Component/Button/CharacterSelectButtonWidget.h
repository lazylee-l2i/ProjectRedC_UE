// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/_Base/Component/BaseButton.h"
#include "CharacterSelectButtonWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UCharacterSelectButtonWidget : public UBaseButton
{
	GENERATED_BODY()
	
		
public:
	virtual void NativeConstruct() override; //런타임용
	virtual void NativeDestruct() override;

public:
	UFUNCTION()
	void HandleClicked();

public:
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Character;
	
	
};
