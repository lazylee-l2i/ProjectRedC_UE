// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RedCProjectileAbility.h"
#include "RedCRangedChargeAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCRangedChargeAttackAbility : public URedCProjectileAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void OnMontageCompleted() override;
	virtual void OnMontageInterrupted() override;
	virtual void OnMontageCancelled() override;
	virtual void OnGameplayEventReceived(const FGameplayEventData Payload) override;
	virtual void OnInputReleased(float TimeHeld) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Montage")
	FRedCMontageInfo MontageInfo;
};
