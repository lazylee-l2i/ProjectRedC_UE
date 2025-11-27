// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RedCAttackAbility.h"
#include "GAS/Abilities/RedCGameplayAbility.h"
#include "RedCProjectileAbility.generated.h"

class UProjectileSpawnEventObject;

USTRUCT()
struct FRedCMontageInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FName TaskInstanceName = FName();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> MontageToPlay = nullptr;

	UPROPERTY()
	float PlayRate = 0.f;

	UPROPERTY(EditDefaultsOnly)
	FName StartSection = FName();
};

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCProjectileAbility : public URedCAttackAbility
{
	GENERATED_BODY()
	
public:
	URedCProjectileAbility();

protected:
	virtual void SpawnProjectile(const UProjectileSpawnEventObject* EventObject);
	
	virtual bool PlayMontageAndWait(const FRedCMontageInfo& MontageInfo);
	virtual void WaitGameplayEvent(const FGameplayTag& EventTag, bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);
	virtual void WaitInputRelease(bool TestAlreadyReleased = false);

	UFUNCTION()
	virtual void OnGameplayEventReceived(const FGameplayEventData Payload);

	UFUNCTION()
	virtual void OnInputReleased(float TimeHeld);

	UFUNCTION()
	virtual void OnMontageCompleted();
	UFUNCTION()
	virtual void OnMontageInterrupted();
	UFUNCTION()
	virtual void OnMontageCancelled();
	UFUNCTION()
	virtual void OnMontageBlendOut();
};
