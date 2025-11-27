// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/RedCEnergySet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


void URedCEnergySet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URedCEnergySet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URedCEnergySet, MaxStamina, COND_None, REPNOTIFY_Always);
}

void URedCEnergySet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	/*if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}*/

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		float BaseValue = GetStaminaAttribute().GetGameplayAttributeData(this)->GetBaseValue();
		BaseValue = FMath::Clamp(BaseValue, 0.f, GetMaxStamina());
		SetStamina(BaseValue);
	}
}

void URedCEnergySet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCEnergySet, Stamina, OldValue);
}

void URedCEnergySet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCEnergySet, MaxStamina, OldValue);
}
