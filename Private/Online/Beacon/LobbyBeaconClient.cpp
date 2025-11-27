// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/Beacon/LobbyBeaconClient.h"

#include "Online/Beacon/LobbyBeaconHostObject.h"
#include "Types/RedCLogChannels.h"


class ALobbyBeaconHostObject;

bool ALobbyBeaconClient::ConnectToLobby(const FString& Address, int32 Port)
{
	FURL URL(nullptr, *Address, TRAVEL_Absolute);
	URL.Port=Port;
	UE_LOG(LogRedCBeacon, Warning, TEXT("URL Is Ready / Init Start"));
	return InitClient(URL);
}

void ALobbyBeaconClient::OnConnected()
{
	Super::OnConnected();
	UE_LOG(LogRedCBeacon, Warning, TEXT("Connect to Lobby Host Success"));
	Server_ReportMatchEnded(TEXT("MatchTick1"), TEXT("TeamA"));
}

void ALobbyBeaconClient::Server_ReportMatchEnded_Implementation(const FString& MatchTicket, const FString& WinnerTeam)
{
	if (AOnlineBeaconHostObject* HostObj = GetBeaconOwner())
	{
		if (ALobbyBeaconHostObject* LobbyHost = Cast<ALobbyBeaconHostObject>(HostObj))
		{
			LobbyHost->ReceiveMatchEnd(MatchTicket, WinnerTeam);
		}
	}
	DestroyBeacon();
}
