// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "UI/Type/HUDType.h"
#include "RedCGameStateBase.generated.h"

enum class EProjectGameMode : uint8;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ARedCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	EProjectGameMode ProjectGameMode = EProjectGameMode::Default;
};
