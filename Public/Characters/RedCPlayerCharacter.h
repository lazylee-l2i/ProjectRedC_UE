// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RedCCharacter.h"
#include "Components/ItemShopComponent.h"
#include "RedCPlayerCharacter.generated.h"

class UActionComponent;
class UCameraComponent;
class USpringArmComponent;

/**
*
*/
UCLASS()
class PROJECTREDC_API ARedCPlayerCharacter : public ARedCCharacter
{
	GENERATED_BODY()
	
public:
	ARedCPlayerCharacter();
	
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	FORCEINLINE USpringArmComponent* GetSpringArmComp() { return SpringArmComp; }
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<UActionComponent> ActionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<UItemShopComponent> ItemShopComp;
	
};