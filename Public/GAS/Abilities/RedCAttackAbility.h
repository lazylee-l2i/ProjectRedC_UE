// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RedCGameplayAbility.h"
#include "Types/RedCAbilityTypes.h"
#include "RedCAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API
URedCAttackAbility : public URedCGameplayAbility
{
	GENERATED_BODY()

public:
	URedCAttackAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnReleased(float TimeHeld);

	FORCEINLINE float GetCoolDownDuration() const {return CoolDownDuration;}
	FORCEINLINE FName GetSkillName() const {return SkillName;}
	FORCEINLINE FGameplayTag GetAbilityInputTag() const {return InputTag;}

protected:
	void ApplyDamageEffect(AActor* HitResult, float DamageAmount);
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual float GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const override;
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	float CoolDownDuration = 0.1f;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	FName SkillName = NAME_None;
};
