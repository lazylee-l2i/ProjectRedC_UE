// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/_Base/Component/BaseButton.h"
#include "MatchingButtonWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UMatchingButtonWidget : public UBaseButton
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override; //외형,속성용 / 에디터 미리보기
	virtual void NativeConstruct() override; //런타임용
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_MenuUI;

private:
	UFUNCTION()
	void HandleClicked();
	
	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();
};
