// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RedCAttackAbility.h"
#include "RedCMeleeAttackAbility.generated.h"

struct FMotionWarpingParam;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCMeleeAttackAbility : public URedCAttackAbility
{
	GENERATED_BODY()

#pragma region Ability Setup
public:
	URedCMeleeAttackAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// AnimMontage와 GameplayEvent Task 생성
	void SetupMeleeAttackTasks();

	// 추가 입력에 대한 Wait Input Task 생성
	void SetupWaitComboInputPress();

	virtual void OnEndAbility() override;
	
#pragma endregion

#pragma region Combo Handle
public:
	static FGameplayTag GetComboChangedEventTag();
	static FGameplayTag GetComboChangedEventEndTag();
	static FGameplayTag GetComboTargetEventTag();

protected:
	UFUNCTION()
	void HandleInputPress(float TimeWaited);
	
	UFUNCTION()
	void ComboChangedEventReceived(FGameplayEventData Payload);
	
	void TryCommitCombo();

private:
	UPROPERTY()
	UGameplayTask* WaitComboInputPressTask;
#pragma endregion

#pragma region Combo Variable
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ComboMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TMap<FName, float> ComboSKillLength;

	void AddMotionWarpingPoint();
	void RemoveMotionWarpingPoint();
	FMotionWarpingParam GenerateMotionWarpingParamByCurrentCombo(float& DistanceMultiplier);
private:
	FName NextComboName;
#pragma endregion

};

