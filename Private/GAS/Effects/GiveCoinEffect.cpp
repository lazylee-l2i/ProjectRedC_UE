// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/GiveCoinEffect.h"

#include "GAS/Attributes/RedCCurrencySet.h"
#include "Types/RedCGameplayTags.h"


UGiveCoinEffect::UGiveCoinEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo CoinModifier;
	CoinModifier.Attribute = URedCCurrencySet::GetCoinAttribute();
	CoinModifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = DataTags::Data_SetByCaller_Coin;
	CoinModifier.ModifierMagnitude = SetByCallerMagnitude;

	Modifiers.Add(CoinModifier);
}
