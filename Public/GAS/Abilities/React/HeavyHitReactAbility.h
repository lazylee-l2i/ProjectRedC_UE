// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/React/RedCHitReactAbility.h"
#include "HeavyHitReactAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UHeavyHitReactAbility : public URedCHitReactAbility
{
	GENERATED_BODY()
	
public:
	UHeavyHitReactAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

private:
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);
};
