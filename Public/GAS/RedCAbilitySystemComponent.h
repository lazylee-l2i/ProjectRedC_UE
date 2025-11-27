// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RedCAbilitySystemComponent.generated.h"

class UProjectileSpawnEventObject;

struct FRedCEffectInfo;
struct FRedCAbilityInfo;
struct FRedCProjectileInfo;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	URedCAbilitySystemComponent();
	
protected:
	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	
public:
	void GrantGameplayAbilities(const TArray<FRedCAbilityInfo>& Abilities);
	void ApplyGameplayEffects(const TArray<FRedCEffectInfo>& Effects);
	
	void SendProjectileSpawnEvent(const FGameplayTag EventTag, const FRedCProjectileInfo& ProjectileInfo);

private:
	UFUNCTION(Server, Reliable)
	void ServerSendProjectileSpawnEvent(const FGameplayTag EventTag, const FRedCProjectileInfo& ProjectileInfo);
};
