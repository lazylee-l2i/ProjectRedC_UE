// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Settings/MainMenuUI.h"

#include "Controllers/RedCPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Manager/UIManagerSubsystem.h"


class ARedCLocalPlayerController;

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Play)	 Button_Play->OnButtonClicked.AddDynamic(this, &UMainMenuUI::OnPlayClicked);
}

void UMainMenuUI::NativeDestruct()
{
	if (Button_Play)      Button_Play->OnButtonClicked.RemoveDynamic(this, &UMainMenuUI::OnPlayClicked); 

	Super::NativeDestruct();
}

void UMainMenuUI::OnPlayClicked()
{
	FString PlayerNameString = PlayerName->GetText().ToString();
	if (PlayerNameString.IsEmpty())
	{
		return;
	}

	FString ServerIPString = ServerIP->GetText().ToString();
	if (ServerIPString.IsEmpty())
	{
		return;
	}
	
	if (ARedCPlayerController* PC = GetOwningPlayer<ARedCPlayerController>())
	{
		PC->Client_StartLoadingWidget();
		if (const ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UUIManagerSubsystem* UIManager = LocalPlayer->GetSubsystem<UUIManagerSubsystem>())
			{
				UIManager->RemoveCurrentMenuHUD(PC);
				UIManager->TempServerIP = ServerIPString;
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("MainMenuUI : %s"), *PlayerNameString);

		PC->ClientTravel(ServerIPString + ":" + "7777" +
			FString::Format(TEXT("?player={0}"), { PlayerNameString }),
			TRAVEL_Absolute
			);
	}
}


