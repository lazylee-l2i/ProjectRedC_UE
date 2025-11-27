// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"
#include "LobbyBeaconHostObject.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ALobbyBeaconHostObject : public AOnlineBeaconHostObject
{
	GENERATED_BODY()
	
public:
	ALobbyBeaconHostObject();

	UFUNCTION()
	void ReceiveMatchEnd(const FString& MatchTicket, const FString& WinnerTeam);
	
	
};
