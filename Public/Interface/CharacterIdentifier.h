// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/RedCAbilityTypes.h"
#include "UObject/Interface.h"

#include "CharacterIdentifier.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterIdentifier : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTREDC_API ICharacterIdentifier
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FGameplayTag GetCharacterTag() const = 0;
	
	virtual uint8 GetCharacterTeamId_UINT8() const = 0;
	virtual bool IsCharacterSameTeam(const ICharacterIdentifier* Other) const = 0;

	// Static Methods
	static FGameplayTag GetCharacterTag(const AActor* Actor);
	static ETeamType GetCharacterTeam(const AActor* Actor);
	static EActorType GetActorType(const AActor* Actor);
};
