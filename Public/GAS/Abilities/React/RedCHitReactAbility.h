// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RedCGameplayAbility.h"
#include "Types/RedCAbilityTypes.h"
#include "RedCHitReactAbility.generated.h"

class UCurveVector;
class UAbilityTask_ApplyRootMotionJumpForce;
class UAbilityTask_ApplyRootMotionMoveToForce;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_MoveToLocation;

UCLASS()
class PROJECTREDC_API URedCHitReactAbility : public URedCGameplayAbility
{
	GENERATED_BODY()
public:
	URedCHitReactAbility();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	void SetTriggerTagParameter(const FGameplayTag& TriggerTag);

#pragma region Hit React Variable
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY()
	mutable FHitReactMessage HitReactMessage = FHitReactMessage();

protected:
	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetHitReactMontage() const;

	const FHitReactMessage& GetHitData() const;
#pragma endregion

#pragma region Hit Data Method
protected:
	UFUNCTION(BlueprintCallable)
	FVector GetHitResultLocation() const;
	
#pragma endregion

#pragma region Task Maker Method
protected:
	UFUNCTION(BlueprintCallable)
	UAbilityTask_ApplyRootMotionMoveToForce* GetApplyRootMotionMoveToTask();

	UFUNCTION(BlueprintCallable)
	UAbilityTask_PlayMontageAndWait* GetMontageTask();

#pragma endregion
};
