// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/Notify/BaseClass/AN_RedCAnimNotify.h"
#include "AN_RedCBattleNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_RedCBattleNotify : public UAN_RedCAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTag EventTag;
	
};
