// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/InvincibleEffect.h"

#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Types/RedCGameplayTags.h"


UInvincibleEffect::UInvincibleEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(2.f);

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = 1;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack;
	
	UTargetTagsGameplayEffectComponent* TargetTagsComponent = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));
	FInheritedTagContainer GrantedTagContainer;
	GrantedTagContainer.Added.AddTag(PlayerStateTags::Player_State_Invincible);
	GrantedTagContainer.CombinedTags.AddTag(PlayerStateTags::Player_State_Invincible);
	TargetTagsComponent->SetAndApplyTargetTagChanges(GrantedTagContainer);
	
	GEComponents.Add(TargetTagsComponent);
}
