// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/RedCHealthSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Frameworks/GameState/GameStateArena.h"
#include "Interface/CharacterIdentifier.h"
#include "Types/RedCLogChannels.h"


void URedCHealthSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URedCHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URedCHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void URedCHealthSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	/*if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}*/

	//UE_LOG(LogRedC, Warning, TEXT("Victim : %s, Attacker : %s"), , *Data.EffectSpec.GetEffectContext().GetInstigator()->GetName())
	AActor* Victim = Data.Target.GetAvatarActor();
	AActor* Attacker = Data.EffectSpec.GetEffectContext().GetInstigator();

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float BaseValue = GetHealthAttribute().GetGameplayAttributeData(this)->GetBaseValue();
		BaseValue = FMath::Clamp(BaseValue, 0.f, GetMaxHealth());
		SetHealth(BaseValue);

		if (BaseValue <= 0.f)
		{
			if (AGameStateArena* GSArena = GetWorld()->GetGameState<AGameStateArena>())
			{
				GSArena->GiveCoinHandle(Attacker, Victim);
			}
		}
	}
}

void URedCHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCHealthSet, Health, OldValue);
}

void URedCHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URedCHealthSet, MaxHealth, OldValue);
}
