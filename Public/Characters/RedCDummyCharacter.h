// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RedCPlayerCharacter.h"
#include "Types/RedCAbilityTypes.h"
#include "RedCDummyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ARedCDummyCharacter : public ARedCPlayerCharacter
{
	GENERATED_BODY()
	
public:
	ARedCDummyCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void InitAbilitySystemComponent() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	void OnRep_AbilitySystemComponent();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
	
	UPROPERTY(ReplicatedUsing = OnRep_AbilitySystemComponent)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
