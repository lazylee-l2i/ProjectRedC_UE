// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/RedCAttributeSet.h"
#include "RedCCurrencySet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCCurrencySet : public URedCAttributeSet
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Coin)
	FGameplayAttributeData Coin;
	ATTRIBUTE_ACCESSORS(URedCCurrencySet, Coin);

private:
	UFUNCTION()
	void OnRep_Coin(const FGameplayAttributeData& OldValue);
};
