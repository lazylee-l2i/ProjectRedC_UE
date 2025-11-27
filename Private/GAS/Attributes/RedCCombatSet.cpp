// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/RedCCombatSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


void URedCCombatSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URedCCombatSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URedCCombatSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URedCCombatSet, Critical, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URedCCombatSet, Evasion, COND_None, REPNOTIFY_Always);
}

void URedCCombatSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	/*if (Data.EvaluatedData.Attribute == GetAttackPowerAttribute())
	{
		SetAttackPower(FMath::Max(GetAttackPower(), 0.f));
	}

	if (Data.EvaluatedData.Attribute == GetDefenseAttribute())
	{
		SetDefense(FMath::Max(GetDefense(), 0.f));
	}

	if (Data.EvaluatedData.Attribute == GetCriticalAttribute())
	{
		SetCritical(FMath::Max(GetCritical(), 0.f));
	}

	if (Data.EvaluatedData.Attribute == GetEvasionAttribute())
	{
		SetEvasion(FMath::Max(GetEvasion(), 0.f));
	}*/

	if (Data.EvaluatedData.Attribute == GetAttackPowerAttribute())
	{
		float BaseValue = GetAttackPowerAttribute().GetGameplayAttributeData(this)->GetBaseValue();
		BaseValue = FMath::Max(BaseValue, 0.f);
		SetAttackPower(BaseValue);
	}

	if (Data.EvaluatedData.Attribute == GetDefenseAttribute())
	{
		float BaseValue = GetDefenseAttribute().GetGameplayAttributeData(this)->GetBaseValue();
		BaseValue = FMath::Max(BaseValue, 0.f);
		SetDefense(BaseValue);
	}

	if (Data.EvaluatedData.Attribute == GetCriticalAttribute())
	{
		float BaseValue = GetCriticalAttribute().GetGameplayAttributeData(this)->GetBaseValue();
		BaseValue = FMath::Max(BaseValue, 0.f);
		SetCritical(BaseValue);
	}

	if (Data.EvaluatedData.Attribute == GetEvasionAttribute())
	{
		float BaseValue = GetEvasionAttribute().GetGameplayAttributeData(this)->GetBaseValue();
		BaseValue = FMath::Max(BaseValue, 0.f);
		SetEvasion(BaseValue);
	}
}

void URedCCombatSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCCombatSet, AttackPower, OldValue);
}

void URedCCombatSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCCombatSet, Defense, OldValue);
}

void URedCCombatSet::OnRep_Critical(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCCombatSet, Critical, OldValue);
}

void URedCCombatSet::OnRep_Evasion(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCCombatSet, Evasion, OldValue);
}
