// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RedCAttackAbility.h"
#include "GAS/Abilities/RedCGameplayAbility.h"
#include "RedCGrabAttackAbility.generated.h"

class UContextualAnimSceneAsset;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCGrabAttackAbility : public URedCAttackAbility
{
	GENERATED_BODY()

#pragma region Ability Setup
public:
	URedCGrabAttackAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> GrabMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> GetUpMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UContextualAnimSceneAsset> GrabSceneAsset;

	UPROPERTY(EditDefaultsOnly)
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier;

private:
	UFUNCTION()
	void OnGrabTarget(FGameplayEventData Payload);

	UFUNCTION()
	void OnGrabEnd(FGameplayEventData Payload);

	UFUNCTION()
	void OnDamageEvent(FGameplayEventData Payload);

	UPROPERTY()
	TObjectPtr<AActor> GrabTarget;
	
#pragma endregion
	
	
};



