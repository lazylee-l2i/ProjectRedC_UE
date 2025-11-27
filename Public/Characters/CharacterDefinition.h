// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/RedCCharacterTypes.h"
#include "CharacterDefinition.generated.h"

class URedCProjectileSet;
class URedCAbilitySet;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UCharacterDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FCharacterMeshInfo CharacterMeshInfo;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<URedCAbilitySet> CharacterAbilitySet;
};
