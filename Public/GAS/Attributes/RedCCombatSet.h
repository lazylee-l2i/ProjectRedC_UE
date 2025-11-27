// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/RedCAttributeSet.h"
#include "RedCCombatSet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCCombatSet : public URedCAttributeSet
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(URedCCombatSet, AttackPower);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(URedCCombatSet, Defense);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Critical)
	FGameplayAttributeData Critical;
	ATTRIBUTE_ACCESSORS(URedCCombatSet, Critical);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Evasion)
	FGameplayAttributeData Evasion;
	ATTRIBUTE_ACCESSORS(URedCCombatSet, Evasion);
	
private:
	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Critical(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Evasion(const FGameplayAttributeData& OldValue);
};
