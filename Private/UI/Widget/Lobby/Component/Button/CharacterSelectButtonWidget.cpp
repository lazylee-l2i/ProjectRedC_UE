// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Lobby/Component/Button/CharacterSelectButtonWidget.h"
#include "Components/Button.h"

void UCharacterSelectButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Character)
	{
		Btn_Character->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
	}
}

void UCharacterSelectButtonWidget::NativeDestruct()
{
	if (Btn_Character)
	{
		Btn_Character->OnClicked.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UCharacterSelectButtonWidget::HandleClicked()
{
	OnButtonClicked.Broadcast();
}



