// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/RedCAbilitySystemComponent.h"
#include "RedCPlayerAbilitySystemComponent.generated.h"

class URedCGameplayAbility;

/**
 * 
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class PROJECTREDC_API URedCPlayerAbilitySystemComponent : public URedCAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);
};
