// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Lobby/CharacterSelectionUI.h"

#include "Characters/RedCPlayerCharacter.h"
#include "Components/WidgetSwitcher.h"
#include "Controllers/RedCPlayerController.h"
#include "Types/RedCLogChannels.h"


void UCharacterSelectionUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (CharacterSelectionBox->Btn_Warrior)
	{
		CharacterSelectionBox->Btn_Warrior->OnButtonClicked.AddDynamic(this, &ThisClass::OnWarriorClicked);
	}
	if (CharacterSelectionBox->Btn_Archor)
	{
		CharacterSelectionBox->Btn_Archor->OnButtonClicked.AddDynamic(this, &ThisClass::OnArchorClicked);
	}
	// if (CharacterSelectionBox->Btn_Magician)
	// {
	// 	CharacterSelectionBox->Btn_Magician->OnButtonClicked.AddDynamic(this, &ThisClass::OnMagicianClicked);
	// }
}

void UCharacterSelectionUI::NativeDestruct()
{
	if (CharacterSelectionBox->Btn_Warrior)
	{
		CharacterSelectionBox->Btn_Warrior->OnButtonClicked.RemoveDynamic(this, &ThisClass::OnWarriorClicked);
	}
	if (CharacterSelectionBox->Btn_Archor)
	{
		CharacterSelectionBox->Btn_Archor->OnButtonClicked.RemoveDynamic(this, &ThisClass::OnArchorClicked);
	}
	// if (CharacterSelectionBox->Btn_Magician)
	// {
	// 	CharacterSelectionBox->Btn_Magician->OnButtonClicked.RemoveDynamic(this, &ThisClass::OnMagicianClicked);
	// }
	
	Super::NativeDestruct();
}

void UCharacterSelectionUI::OnWarriorClicked()
{
	ARedCPlayerController* PC = GetOwningPlayer<ARedCPlayerController>();

	// 0 : Warrior
	PC->Server_PlayerPickCharacter(0);
	
	UE_LOG(LogRedCWidget, Warning, TEXT("Warrior OnWarriorClicked"));
	WigetSwitcher_CharacterImg->SetActiveWidgetIndex(0);
}

void UCharacterSelectionUI::OnArchorClicked()
{
	ARedCPlayerController* PC = GetOwningPlayer<ARedCPlayerController>();

	// 1 : Archor
	PC->Server_PlayerPickCharacter(1);
	
	UE_LOG(LogRedCWidget, Warning, TEXT("Warrior OnArchorClicked"));
	WigetSwitcher_CharacterImg->SetActiveWidgetIndex(1);
}

// void UCharacterSelectionUI::OnMagicianClicked()
// {
// 	ARedCPlayerController* PC = GetOwningPlayer<ARedCPlayerController>();
//
// 	// 2 : Magician
// 	PC->Server_PlayerPickCharacter(2);
// 	
// 	UE_LOG(LogRedCWidget, Warning, TEXT("Warrior OnMagicianClicked"));
// 	WigetSwitcher_CharacterImg->SetActiveWidgetIndex(2);
// }


