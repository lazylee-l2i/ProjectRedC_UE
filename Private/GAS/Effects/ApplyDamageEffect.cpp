// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/ApplyDamageEffect.h"

#include "GAS/Attributes/RedCHealthSet.h"
#include "GAS/Effects/DamageCalculation.h"
#include "Types/RedCGameplayTags.h"

/*================================================================
사용법 (in character class)

FGameplayEffectContextHandle DamageEffectContext = ASC->MakeEffectContext();
DamageEffectContext.AddSourceObject(this);

FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
	UApplyDamageEffect::StaticClass(),
	1.0f,
	DamageEffectContext
);

if (SpecHandle.IsValid())
{
	SpecHandle.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_Health, {float Damage});
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
 ================================================================*/
UApplyDamageEffect::UApplyDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	FGameplayEffectExecutionDefinition ExecutionDef;
	ExecutionDef.CalculationClass = UDamageCalculation::StaticClass();

	ExecutionDef.CalculationModifiers.Empty();
	
	Executions.Add(ExecutionDef);
}
