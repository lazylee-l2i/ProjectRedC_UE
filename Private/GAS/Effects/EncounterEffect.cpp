// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/EncounterEffect.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Types/RedCGameplayTags.h"

/*================================================================
사용법 (in character class)

FGameplayEffectContextHandle EncounterEffectContext = ASC->MakeEffectContext();
EncounterEffectContext.AddSourceObject(this);

FGameplayEffectSpecHandle EncounterSpecHandle = ASC->MakeOutgoingSpec(
	UEncounterEffect::StaticClass(),
	1.0f,
	EncounterEffectContext
);

if (EncounterSpecHandle.IsValid())
{
	ASC->ApplyGameplayEffectSpecToSelf(*EncounterSpecHandle.Data.Get());
}
 ================================================================*/
UEncounterEffect::UEncounterEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(15.f);

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = 1;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack;
	
	EffectTag = PlayerStateTags::Player_Mode_InCombat;
	
	UTargetTagsGameplayEffectComponent* TargetTagsComponent = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));
	FInheritedTagContainer GrantedTagContainer;
	GrantedTagContainer.Added.AddTag(EffectTag);
	GrantedTagContainer.CombinedTags.AddTag(EffectTag);

	TargetTagsComponent->SetAndApplyTargetTagChanges(GrantedTagContainer);
	GEComponents.Add(TargetTagsComponent);
}
