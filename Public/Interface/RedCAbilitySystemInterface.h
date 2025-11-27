// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "RedCAbilitySystemInterface.generated.h"

class UCrosshairComponent;

UINTERFACE(MinimalAPI)
class URedCAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTREDC_API IRedCAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCrosshairComponent* GetCrosshairComponent() const;
};
