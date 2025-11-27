// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/Beacon/LobbyBeaconHostObject.h"

#include "Frameworks/RedCGameModeBase.h"
#include "Frameworks/GameMode/GameModeLobby.h"
#include "Online/Beacon/LobbyBeaconClient.h"
#include "Types/RedCLogChannels.h"


ALobbyBeaconHostObject::ALobbyBeaconHostObject()
{
	// 이 클래스(HostObject)가 받을 클라이언트 지정
	ClientBeaconActorClass = ALobbyBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();
}

void ALobbyBeaconHostObject::ReceiveMatchEnd(const FString& MatchTicket, const FString& WinnerTeam)
{
	UE_LOG(LogRedCBeacon, Warning, TEXT(
		"HostObject Received MatchTicket : %s / Winner : %s"), *MatchTicket, *WinnerTeam);
	
	if (AGameModeLobby* GM = Cast<AGameModeLobby>(GetWorld()->GetAuthGameMode()))
	{
		GM->LobbyReceivedEndGame();
	}
}
