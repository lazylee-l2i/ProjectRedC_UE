// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/CharacterSelectBox.h"
#include "UI/Widget/_Base/BaseInGameHUDWidget.h"
#include "UI/Widget/_Base/Component/BaseCountDown.h"
#include "CharacterSelectionUI.generated.h"

class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UCharacterSelectionUI : public UBaseInGameHUDWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* WigetSwitcher_CharacterImg;
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta=(BindWidget))
	UCharacterSelectBox* CharacterSelectionBox;
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UBaseCountDown* CharacterSelectionCountDown;
	
	UFUNCTION()
	void OnWarriorClicked();
	UFUNCTION()
	void OnArchorClicked();
	// UFUNCTION()
	// void OnMagicianClicked();
	
	
};
