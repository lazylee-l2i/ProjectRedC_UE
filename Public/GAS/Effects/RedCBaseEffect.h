// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RedCBaseEffect.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCBaseEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	FORCEINLINE FGameplayTag GetEffectTag() const { return EffectTag; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect Tag")
	FGameplayTag EffectTag;
	
};
