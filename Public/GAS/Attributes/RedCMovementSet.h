// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/RedCAttributeSet.h"
#include "RedCMovementSet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCMovementSet : public URedCAttributeSet
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(URedCMovementSet, MovementSpeed);

private:
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);
};
