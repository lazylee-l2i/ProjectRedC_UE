// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RedCGameplayAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_ApplyRootMotionJumpForce;
class UAbilityTask_ApplyRootMotionMoveToForce;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URedCGameplayAbility();
	
	UPROPERTY(EditDefaultsOnly, Category = "Input Tag")
	FGameplayTag InputTag;

protected:
	class UAnimInstance* GetOwnerAnimInstance() const;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UGameplayTask>> RemainTasks;

	UFUNCTION(Blueprintable)
	void EraseRemainTasks();

	UFUNCTION(Blueprintable)
	void AddWatchedTask(UGameplayTask* Task);

	UFUNCTION(BlueprintCallable)
	virtual void OnEndAbility();
};
