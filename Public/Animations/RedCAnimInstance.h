// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RedCAnimInstance.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed > 0.f; }

	UFUNCTION(BlueprintPure, Meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsSprinting() const { return Speed > 800.f; }

	UFUNCTION(BlueprintPure, Meta = (BlueprintThreadSafe))
	bool IsOnGround() const;

	UFUNCTION(BlueprintPure, Meta = (BlueprintThreadSafe))
	bool IsFalling() const;
protected:
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeUpdateSpeedAndDirection(float DeltaSeconds);
	
private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float Direction = 0.f;
};
