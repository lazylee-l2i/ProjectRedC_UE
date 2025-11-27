// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/WorldSettings.h"
#include "RedCWorldSettings.generated.h"

UENUM()
enum class ELevelType : uint8
{
	None,
	MainMenu,
	Lobby,
	Arena,
	Debug
};

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ARedCWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Custom Values")
	ELevelType LevelType;
	
	UPROPERTY(EditAnywhere, Category = "Custom Values", Meta = (ClampMin = 0, EditCondition = "LevelType == ELevelType::Arena"))
	int32 MaxNumberOfPlayers;

	UPROPERTY(EditAnywhere, Category = "Custom Values", Meta = (Categories = "Character", EditCondition = "LevelType == ELevelType::Debug"))
	FGameplayTag DebugCharacterTag;
};
