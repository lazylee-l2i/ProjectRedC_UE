// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RedCGameplayAbility.h"
#include "WheelUpAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UWheelUpAbility : public URedCGameplayAbility
{
	GENERATED_BODY()

public:
	UWheelUpAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> WheelUpMontage;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> WheelUpEffect;
};
