// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffect.h"
#include "AN_RedCAnimNotify.h"

#include "AN_RedCTriggerGE.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_RedCTriggerGE : public UAN_RedCAnimNotify
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> GameplayEffect;
	
};
