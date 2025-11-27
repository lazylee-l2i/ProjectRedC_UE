// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RedCPlayerStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


ARedCPlayerStateBase::ARedCPlayerStateBase()
{
	NetUpdateFrequency = 100.f;
	MinNetUpdateFrequency = 66.f;
}

void ARedCPlayerStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARedCPlayerStateBase, PlayerTravelInfo);
}

FString ARedCPlayerStateBase::BuildTravelOptions(const FString& MatchID, int32 TeamID)
{
	// 아무것도 선택하지 않았을 시 워리어(CharacterID : 0) 기본선택
	if (PlayerTravelInfo.CharacterID == FString())
	{
		PlayerTravelInfo.CharacterID = "0";
	}
	
	FString Options = FString::Format(TEXT("?match={0}?team={1}?char={2}?player={3}"),
		{
			PlayerTravelInfo.MatchID = MatchID,
			PlayerTravelInfo.TeamID = FString::FromInt(TeamID),
			PlayerTravelInfo.CharacterID,
			PlayerTravelInfo.PlayerName
		});
	
	return Options;
}

void ARedCPlayerStateBase::ParseTravelOptions(const FString& Options)
{
	PlayerTravelInfo.MatchID = UGameplayStatics::ParseOption(Options, TEXT("match"));
	PlayerTravelInfo.TeamID = UGameplayStatics::ParseOption(Options, TEXT("team"));
	PlayerTravelInfo.CharacterID = UGameplayStatics::ParseOption(Options, TEXT("char"));
	PlayerTravelInfo.PlayerName = UGameplayStatics::ParseOption(Options, TEXT("player"));
}

void ARedCPlayerStateBase::SetPlayerTravelInfo_CharacterID(int32 CharacterID)
{
	PlayerTravelInfo.CharacterID = FString::FromInt(CharacterID);
}

void ARedCPlayerStateBase::SetPlayerTravelInfo_TeamID(int32 TeamID)
{
	PlayerTravelInfo.TeamID = FString::FromInt(TeamID);
}

FRedCTravelInfo ARedCPlayerStateBase::GetPlayerTravelInfo() const
{
	return PlayerTravelInfo;
}
