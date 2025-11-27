#pragma once

#include "CoreMinimal.h"
#include "RedCTravelInfo.generated.h"

USTRUCT()
struct FRedCTravelInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString MatchID = FString();
	
	UPROPERTY()
	FString TeamID = FString();
	
	UPROPERTY()
	FString CharacterID = FString();

	UPROPERTY()
	FString PlayerName = FString();
};
