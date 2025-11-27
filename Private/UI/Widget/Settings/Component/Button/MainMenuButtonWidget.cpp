// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Settings/Component/Button/MainMenuButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


void UMainMenuButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Text_MenuButtonName)
	{
		Text_MenuButtonName->SetText(ButtonName);
		Text_MenuButtonName->SetFont(ButtonFont);
	}
	if (Button_MenuUI)
	{
		Button_MenuUI->SetBackgroundColor(ButtonBackgroundColor);
	}
}

void UMainMenuButtonWidget::NativeConstruct()
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

void UMainMenuButtonWidget::NativeDestruct()
{
	if (Button_MenuUI)
	{
		Button_MenuUI->OnClicked.RemoveAll(this);
		Button_MenuUI->OnHovered.RemoveAll(this);
		Button_MenuUI->OnUnhovered.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UMainMenuButtonWidget::HandleClicked()
{
	OnButtonClicked.Broadcast();
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}
}

void UMainMenuButtonWidget::HandleHovered()
{
	Button_MenuUI->SetBackgroundColor(ButtonBackgroundColor_Hover);
	if (HoverSound)
	{
		UGameplayStatics::PlaySound2D(this, HoverSound);
	}
}

void UMainMenuButtonWidget::HandleUnhovered()
{
	Button_MenuUI->SetBackgroundColor(ButtonBackgroundColor_Unhover);
}
