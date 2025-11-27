// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"

class URedCGameplayAbility;
class UGameplayEffect;

struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREDC_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActionComponent();

	void BindInputActions(UInputComponent* PlayerInputComponent);

private:
	/* Native Input Event Methods */
	UFUNCTION()
	void LookAction(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void MoveAction(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void MoveDoneAction();

	UFUNCTION()
	void LMouseAction(const FInputActionValue& InputActionValue);
	bool bIsLMousePressed = false;

	UFUNCTION()
	void RMouseAction(const FInputActionValue& InputActionValue);
	bool bIsRMousePressed = false;

	UFUNCTION()
	void LShiftAction(const FInputActionValue& InputActionValue);
	bool bIsLShiftPressed = false;

	/* Ability Input Event Methods */
	UFUNCTION()
	void AbilityInputPressed(FGameplayTag InputTag);

	UFUNCTION()
	void AbilityInputReleased(FGameplayTag InputTag);
};
