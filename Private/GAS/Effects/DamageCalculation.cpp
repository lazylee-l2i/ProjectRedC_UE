// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/DamageCalculation.h"

#include "GAS/Attributes/RedCCombatSet.h"
#include "GAS/Attributes/RedCHealthSet.h"
#include "GameplayEffect.h"
#include "Controllers/RedCPlayerController.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"

UDamageCalculation::UDamageCalculation()
{
	// Source - 공격자
	AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(URedCCombatSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
	CriticalDef = FGameplayEffectAttributeCaptureDefinition(URedCCombatSet::GetCriticalAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);

	// Target - 피격자
	HealthDef = FGameplayEffectAttributeCaptureDefinition(URedCHealthSet::GetHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	DefenseDef = FGameplayEffectAttributeCaptureDefinition(URedCCombatSet::GetDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	EvasionDef = FGameplayEffectAttributeCaptureDefinition(URedCCombatSet::GetEvasionAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	
	RelevantAttributesToCapture.Add(AttackPowerDef);
	RelevantAttributesToCapture.Add(CriticalDef);
	RelevantAttributesToCapture.Add(HealthDef);
	RelevantAttributesToCapture.Add(DefenseDef);
	RelevantAttributesToCapture.Add(EvasionDef);
}

void UDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	if (not TargetASC || not SourceASC)
	{
		return;
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluationParams;

	// Source - 공격자
	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AttackPowerDef, EvaluationParams, AttackPower);
	float Critical = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CriticalDef, EvaluationParams, Critical);

	// Target - 피격자
	float Health = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthDef, EvaluationParams, Health);
	float Defense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DefenseDef, EvaluationParams, Defense);
	//UE_LOG(LogRedC, Warning, TEXT("Calculation Defense : %f"), Defense);
	float Evasion = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EvasionDef, EvaluationParams, Evasion);

	float SkillPower = Spec.GetSetByCallerMagnitude(DataTags::Data_SetByCaller_SkillPower);
	float SkillMultiplier = Spec.GetSetByCallerMagnitude(DataTags::Data_SetByCaller_SkillMultiplier);

	float BaseDamage = (AttackPower * SkillMultiplier + SkillPower) * (1.f - Defense * 0.01f * 0.9f);

	float Multiplier = 1.f;
	float FinalDamage = BaseDamage * Multiplier;

	// todo - 본인 데미지 cal 계산결과값 여기서 받아오면됨 / FinalDamage
	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(URedCHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, -FinalDamage));

		// 2) 공격자만 보이게 하는 GameplayCue 실행 (서버→공격자 소유 클라)
		//    위치는 '상대방(피격자) 위치' 또는 히트지점으로 설정
		FGameplayCueParameters GCParams;
		
		// (가) 히트 결과가 있으면 그걸 우선 사용
		if (const FGameplayEffectContextHandle& Ctx = Spec.GetContext(); Ctx.IsValid())
		{
			if (const FHitResult* HR = Ctx.GetHitResult())
			{
				GCParams.Location = HR->ImpactPoint;
				GCParams.Normal   = HR->ImpactNormal;
				GCParams.PhysicalMaterial = HR->PhysMaterial.Get();
			}
		}
		// (나) 히트가 없다면 피격자 아바타 위치 사용
		if (!GCParams.Location.IsNearlyZero())
		{
			// 이미 히트 지점 세팅됨
		}
		else if (AActor* TargetAvatar = TargetASC->GetAvatarActor_Direct())
		{
			GCParams.Location = TargetAvatar->GetActorLocation();
		}
		
		// 숫자/메타 정보
		GCParams.RawMagnitude = FinalDamage; // GCN에서 데미지 숫자로 사용
		GCParams.Instigator   = SourceASC->GetAvatarActor_Direct();
		GCParams.EffectCauser = Spec.GetEffectContext().GetEffectCauser(); // 무기/스킬 액터 있으면 자동 전달
		GCParams.SourceObject = Spec.GetEffectContext().GetSourceObject();
		
		// 핵심: **Target이 아니라 Source(공격자) ASC에 Execute**
		TargetASC->ExecuteGameplayCue(GameplayCueTags::GameplayCue_Shared_DamageNumber, GCParams);
	}
}
