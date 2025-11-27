// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/SkillCoolDownEffect.h"

#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Types/RedCGameplayTags.h"


USkillCoolDownEffect::USkillCoolDownEffect()
{
	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = DataTags::Data_SetByCaller_CoolDown;
	
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);
}
