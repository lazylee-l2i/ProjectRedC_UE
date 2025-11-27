// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/RedCAttributeSet.h"

#include "GAS/RedCAbilitySystemComponent.h"


URedCAbilitySystemComponent* URedCAttributeSet::GetRedCAbilitySystemComponent() const
{
	return Cast<URedCAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

void URedCAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 이하 내용은 자식 클래스에서 구현

	/* Attribute에 대한 Replicate 설정 예시 (#include "Net/UnrealNetwork.h") */
	/* Ex. PowerClass에 있는 Power는 조건 없이 항상 복제 되어야 함 */
	/*
		DOREPLIFETIME_CONDITION_NOTIFY(UPowerClass, Power, COND_None, REPNOTIFY_Always);
	*/
}

void URedCAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 이하 내용은 자식 클래스에서 구현

	/* GE 적용 시 Attribute에 대한 최소값, 최대값 설정 예시 (#include "GameplayEffectExtension.h") */
	/* Ex. Power는 0 아래로 내려갈 수 없고 MaxPower 보다 위로 올라갈 수 없음 */
	/*
		if (Data.EvaluatedData.Attribute == GetPowerAttribute())
		{
			SetPower(FMath::Clamp(GetPower(), 0.f, GetMaxPower()));
		}
	*/
}
