// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/RedCMovementSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"


void URedCMovementSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(URedCMovementSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void URedCMovementSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	/*if (Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
	{
		SetMovementSpeed(FMath::Max(GetMovementSpeed(), 0.f));
	}
	*/

	if (Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
	{
		float BaseValue = GetMovementSpeedAttribute().GetGameplayAttributeData(this)->GetBaseValue();
		BaseValue = FMath::Max(BaseValue, 0.f);
		SetMovementSpeed(BaseValue);
	}
}

void URedCMovementSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCMovementSet, MovementSpeed, OldValue);
}
