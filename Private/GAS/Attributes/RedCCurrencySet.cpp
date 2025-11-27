// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/RedCCurrencySet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"


void URedCCurrencySet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URedCCurrencySet, Coin, COND_None, REPNOTIFY_Always);
}

void URedCCurrencySet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	/*if (Data.EvaluatedData.Attribute == GetCoinAttribute())
	{
		SetCoin(FMath::Max(GetCoin(), 0.f));
	}*/

	if (Data.EvaluatedData.Attribute == GetCoinAttribute())
	{
		float BaseValue = GetCoinAttribute().GetGameplayAttributeData(this)->GetBaseValue();
		BaseValue = FMath::Max(BaseValue, 0.f);
		SetCoin(BaseValue);
	}
}

void URedCCurrencySet::OnRep_Coin(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCCurrencySet, Coin, OldValue);
}
