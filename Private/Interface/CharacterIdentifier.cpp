// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/CharacterIdentifier.h"
#include "GameplayTagContainer.h"
#include "Types/RedCGameplayTags.h"


// Actor의 Tag값을 불러오는 메소드
FGameplayTag ICharacterIdentifier::GetCharacterTag(const AActor* Actor)
{
	if (const ICharacterIdentifier* Identifier = Cast<ICharacterIdentifier>(Actor))
	{
		// 워 슈발 이런식의 작성도 가능하네?
		if (FGameplayTag Tag = Identifier->GetCharacterTag(); Tag.IsValid())
		{
			return Tag;
		}
	}
	return CharacterTags::Character_None.GetTag();
}

// Actor의 Team값을 불러오는 메소드
ETeamType ICharacterIdentifier::GetCharacterTeam(const AActor* Actor)
{
	if (const ICharacterIdentifier* Identifier = Cast<ICharacterIdentifier>(Actor))
	{
		return static_cast<ETeamType>(Identifier->GetCharacterTeamId_UINT8());
	}
	return ETeamType::None;
}

// Actor의 타입을 불러오는 메소드
EActorType ICharacterIdentifier::GetActorType(const AActor* Actor)
{
	FGameplayTag Tag = GetCharacterTag(Actor);
	if (Tag == CharacterTags::Character_Warrior || Tag == CharacterTags::Character_Archer)
	{
		return EActorType::Hero;
	}
	else if (Tag == CharacterTags::Character_Minion)
	{
		return EActorType::Minion;
	}
	else if (Tag == CharacterTags::Character_Tower)
	{
		return EActorType::Tower;
	}
	else
	{
		return EActorType::None;
	}
}
