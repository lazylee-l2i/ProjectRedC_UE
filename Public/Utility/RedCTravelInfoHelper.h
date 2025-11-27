// RedCTravelInfoHelper.h
#pragma once

#include "CoreMinimal.h"
#include "Types/RedCTravelInfo.h"

namespace RedCTravelHelper
{
	static FName ConvertCharacterToKorean(const FString& CharacterID)
	{
		static const TMap<FString, FName> CharacterNameMap = {
			{ TEXT("Character.Mage"),     FName(TEXT("페이즈")) },
			{ TEXT("Character.Warrior"),  FName(TEXT("그레이스톤")) },
			{ TEXT("Character.Archer"),   FName(TEXT("스패로우")) },
		};

		if (const FName* Found = CharacterNameMap.Find(CharacterID))
			return *Found;

		return FName(TEXT("알 수 없음"));
	}

	static FName ConvertTeamToKorean(const FString& TeamID)
	{
		if (TeamID == TEXT("1"))
			return FName(TEXT("레드팀"));
		if (TeamID == TEXT("2"))
			return FName(TEXT("블루팀"));
		return FName(TEXT("무소속"));
	}

	static FName ConvertMatchToKorean(const FString& MatchID)
	{
		return FName(*FString::Printf(TEXT("매치_%s"), *MatchID));
	}

	static void GetKoreanInfo(const FRedCTravelInfo& Info, FName& OutMatch, FName& OutTeam, FName& OutCharacter)
	{
		OutMatch     = ConvertMatchToKorean(Info.MatchID);
		OutTeam      = ConvertTeamToKorean(Info.TeamID);
		OutCharacter = ConvertCharacterToKorean(Info.CharacterID);
	}
}
