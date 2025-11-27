// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameState/GameStateLobby.h"

#include "Frameworks/PlayerState/PlayerStateLobby.h"
#include "Net/UnrealNetwork.h"



void AGameStateLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGameStateLobby, PressedCount);
	DOREPLIFETIME(AGameStateLobby, MatchingLobbyCountDown);
	DOREPLIFETIME(AGameStateLobby, MatchingCharacterSelectionCountDown);
	DOREPLIFETIME(AGameStateLobby, MatchStarted);
	DOREPLIFETIME(AGameStateLobby, MatchedPlayerInfo);
}


void AGameStateLobby::OnRep_PressedCount()
{
	OnPressedCountChanged.Broadcast(PressedCount);
}


void AGameStateLobby::MakeTeam(TArray<TWeakObjectPtr<ARedCPlayerController>> PlayerControllers)
{
	int TempIndex = -1;
	for (auto& PC : PlayerControllers)
	{
		if (APlayerStateLobby* PS = PC->GetPlayerState<APlayerStateLobby>())
		{
			if (TempIndex == -1)
			{
				PS->TeamIDForLobby = 0;
				MatchedPlayerInfo.Add(FRedCCharacterSelectionInfo({0, PC}));
				TempIndex *= -1;
			}
			else if (TempIndex == 1)
			{
				PS->TeamIDForLobby = 1;
				MatchedPlayerInfo.Add(FRedCCharacterSelectionInfo({1, PC}));
				TempIndex *= -1;
			}
		}
	}
}

void AGameStateLobby::StartLobbyCountDown()
{
	if (!HasAuthority()) return;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&AGameStateLobby::HandleLobbyCountDown,
		1,
		true
	);
}

void AGameStateLobby::HandleLobbyCountDown()
{
	if (!HasAuthority()) return;
	MatchingLobbyCountDown--;
	if (MatchingLobbyCountDown+ShowMatchStartComment <=0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		OnMatchingCountDownFinished.Broadcast();
		MatchingLobbyCountDown = ResetLobbyCountDown;
	}
}

void AGameStateLobby::OnRep_LobbyCountDown()
{
	OnMatchingCountDownTicking.Broadcast(MatchingLobbyCountDown);
}



void AGameStateLobby::StartCharacterSelectionCountDown()
{
	if (!HasAuthority()) return;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&AGameStateLobby::HandleCharacterSelectionCountDown,
		1,
		true
	);
}

void AGameStateLobby::HandleCharacterSelectionCountDown()
{
	if (!HasAuthority()) return;
	MatchingCharacterSelectionCountDown--;
	if (MatchingCharacterSelectionCountDown+ShowGameStartComment <=0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		OnMatchingCharacterSelectionCountDownFinished.Broadcast();
	}
}

void AGameStateLobby::OnRep_CharacterSelectionCountDown()
{
	OnMatchingCharacterSelectionCountDownTicking.Broadcast(MatchingCharacterSelectionCountDown);
}
