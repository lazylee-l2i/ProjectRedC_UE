// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RedCGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	URedCGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;
	virtual void Shutdown() override;
};
