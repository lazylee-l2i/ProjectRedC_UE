// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Lobby/Component/MatchingWidget.h"

#include "Controllers/RedCPlayerController.h"
#include "Types/RedCLogChannels.h"


void UMatchingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Button_Matching)
	{
		Button_Matching->OnButtonClicked.AddDynamic(this, &ThisClass::UMatchingWidget::OnMatchClicked);
	}
}

void UMatchingWidget::NativeDestruct()
{
	if (Button_Matching) Button_Matching->OnButtonClicked.RemoveDynamic(this, &ThisClass::UMatchingWidget::OnMatchClicked);
	
	Super::NativeDestruct();
}

void UMatchingWidget::OnMatchClicked()
{
	if (ARedCPlayerController* PC = GetOwningPlayer<ARedCPlayerController>())
	{
		PC->RegisterPlayerController();
	}
}