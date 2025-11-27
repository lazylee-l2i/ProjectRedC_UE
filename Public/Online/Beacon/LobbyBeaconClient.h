// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "LobbyBeaconClient.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ALobbyBeaconClient : public AOnlineBeaconClient
{
	GENERATED_BODY()
	
public:
    // 로비접속
    UFUNCTION()
    bool ConnectToLobby(const FString& Address, int32 Port);
        	
    // 로비접속 후 자동호출
    virtual void OnConnected() override;
    
	// 전투서버 -> 로비서버 RPC
	UFUNCTION(Server,Reliable)
	void Server_ReportMatchEnded(const FString& MatchTicket, const FString& WinnerTeam);

};
