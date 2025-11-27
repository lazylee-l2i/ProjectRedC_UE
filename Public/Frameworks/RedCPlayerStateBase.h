// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Types/RedCTravelInfo.h"
#include "RedCPlayerStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ARedCPlayerStateBase : public APlayerState
{
	GENERATED_BODY()

public:
	ARedCPlayerStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void ParseTravelOptions(const FString& Options);
	virtual FString BuildTravelOptions(const FString& MatchID, int32 TeamID);

	// Setter
	virtual void SetPlayerTravelInfo_CharacterID(int32 CharacterID);
	virtual void SetPlayerTravelInfo_TeamID(int32 TeamID);
	void SetPlayerTravelInfo(const FRedCTravelInfo& TravelInfo) { PlayerTravelInfo = TravelInfo; }
	
	// Getter
	FRedCTravelInfo GetPlayerTravelInfo() const;
	
protected:
	UPROPERTY(Replicated)
	FRedCTravelInfo PlayerTravelInfo;
};
