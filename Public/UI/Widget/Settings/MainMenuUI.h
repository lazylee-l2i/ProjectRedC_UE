// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/Button/MainMenuButtonWidget.h"
#include "Components/EditableTextBox.h"
#include "UI/Widget/_Base/BaseMenuWidget.h"
#include "MainMenuUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UMainMenuUI : public UBaseMenuWidget
{
	GENERATED_BODY()

	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* EnterMainMenuFadeInAnim;
	
	// 버튼
	UPROPERTY(meta=(BindWidget))
	UMainMenuButtonWidget* Button_Play;

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* PlayerName;

	// (디버깅)서버 접속용 텍스트 블록
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* ServerIP;

private:
	// 버튼 콜백
	UFUNCTION()
	void OnPlayClicked();
};
