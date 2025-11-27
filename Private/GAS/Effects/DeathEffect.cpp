// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/DeathEffect.h"

#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Types/RedCGameplayTags.h"

/*================================================================
사용법 (in character class)

FGameplayEffectContextHandle DeathEffectContext = ASC->MakeEffectContext();
DeathEffectContext.AddSourceObject(this);

FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
	UDeathEffect::StaticClass(),
	1.0f,
	DeathEffectContext
);

if (SpecHandle.IsValid())
{
	SpecHandle.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_RespawnTimer, {float Respawn Time});
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
 ================================================================*/
UDeathEffect::UDeathEffect()
{
	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = DataTags::Data_SetByCaller_RespawnTimer;
	
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	EffectTag = PlayerStateTags::Player_State_Death;

	UTargetTagsGameplayEffectComponent* TargetTagsComponent = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));
	FInheritedTagContainer GrantedTagContainer;
	GrantedTagContainer.Added.AddTag(EffectTag);
	GrantedTagContainer.CombinedTags.AddTag(EffectTag);

	TargetTagsComponent->SetAndApplyTargetTagChanges(GrantedTagContainer);
	GEComponents.Add(TargetTagsComponent);
}
