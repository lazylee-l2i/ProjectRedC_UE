// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Lobby/Component/MatchingButtonWidget.h"

#include "Components/Button.h"


void UMatchingButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Button_MenuUI)
	{
		Button_MenuUI->SetBackgroundColor(ButtonBackgroundColor);
	}
}

void UMatchingButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_MenuUI)
	{
		Button_MenuUI->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
	}
	
	if (Button_MenuUI && bUseHoverStyle)
	{
		Button_MenuUI->OnHovered.AddDynamic(this, &ThisClass::HandleHovered);
		Button_MenuUI->OnUnhovered.AddDynamic(this, &ThisClass::HandleUnhovered);
	}
}

void UMatchingButtonWidget::NativeDestruct()
{
	if (Button_MenuUI)
	{
		Button_MenuUI->OnClicked.RemoveAll(this);
		Button_MenuUI->OnHovered.RemoveAll(this);
		Button_MenuUI->OnUnhovered.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UMatchingButtonWidget::HandleClicked()
{
	OnButtonClicked.Broadcast();
}

void UMatchingButtonWidget::HandleHovered()
{
	Button_MenuUI->SetBackgroundColor(ButtonBackgroundColor_Hover);
}

void UMatchingButtonWidget::HandleUnhovered()
{
	Button_MenuUI->SetBackgroundColor(ButtonBackgroundColor_Unhover);
}
