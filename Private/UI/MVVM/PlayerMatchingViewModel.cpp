// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MVVM/PlayerMatchingViewModel.h"

#include "Controllers/RedCPlayerController.h"
#include "Frameworks/GameMode/GameModeLobby.h"
#include "Frameworks/GameState/GameStateLobby.h"
#include "Frameworks/PlayerState/PlayerStateLobby.h"
#include "Kismet/GameplayStatics.h"
#include "Types/RedCLogChannels.h"


void UPlayerMatchingViewModel::BindToLocalWorld(UObject* WorldContext)
{
	UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	if (!World || World->IsNetMode(NM_DedicatedServer)) return;

	if (ARedCPlayerController* PC = Cast<ARedCPlayerController>(UGameplayStatics::GetPlayerController(World, 0)))
	{
		if (APlayerStateLobby* PS = PC->GetPlayerState<APlayerStateLobby>())
		{
			UE_MVVM_SET_PROPERTY_VALUE(MatchingCompleted, PS->MatchingCompleted);
			PS->OnMatchingCompleted.AddUObject(this, &ThisClass::HandleMatchingCompleted);
		}
	}
	

	if (AGameStateLobby* GS = World->GetGameState<AGameStateLobby>())
	{
		// 초기 동기화
		UE_MVVM_SET_PROPERTY_VALUE(PressedCount, GS->PressedCount);
		UE_MVVM_SET_PROPERTY_VALUE(LobbyCountDown, GS->MatchingLobbyCountDown);
		UE_MVVM_SET_PROPERTY_VALUE(CharacterSelectionCountDown, GS->MatchingCharacterSelectionCountDown);
		const AGameModeLobby* GM = Cast<AGameModeLobby>(GS->GetDefaultGameMode());
		UE_MVVM_SET_PROPERTY_VALUE(PlayerTarget, GM->GetTargetPlayers());

		// RepNotify → 델리게이트 통해 변경 구독
		GS->OnPressedCountChanged.AddUObject(this, &ThisClass::HandlePressedCountChanged);
		GS->OnMatchingCountDownTicking.AddUObject(this, &ThisClass::HandleMatchingCountDownChanged);
		GS->OnMatchingCharacterSelectionCountDownTicking.AddUObject(this, &ThisClass::HandleMatchingCharacterSelectionCountDownChanged);
	}
}

void UPlayerMatchingViewModel::HandlePressedCountChanged(int32 NewCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(PressedCount, NewCount);
}

void UPlayerMatchingViewModel::HandleMatchingCompleted(bool NewBool)
{
	UE_MVVM_SET_PROPERTY_VALUE(MatchingCompleted, NewBool);
}

void UPlayerMatchingViewModel::HandleMatchingCountDownChanged(int32 NewCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(LobbyCountDown, NewCount);
}

void UPlayerMatchingViewModel::HandleMatchingCharacterSelectionCountDownChanged(int NewCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(CharacterSelectionCountDown, NewCount);
}

