// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/NotifyState/ANS_GameplayEffect.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Types/RedCGameplayTags.h"


void UANS_GameplayEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (not NotifyValidationCheck(MeshComp))
	{
		return;
	}
	
	if (MeshComp->GetOwner()->HasAuthority())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
		{
			if (ASC->HasMatchingGameplayTag(EffectTag))
			{
				return;
			}
			else
			{
				ActiveEffectHandle = ASC->ApplyGameplayEffectToSelf(
					GameplayEffect.GetDefaultObject(), 0, ASC->MakeEffectContext()
				);
			}
		}
		
	}
}

void UANS_GameplayEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (not NotifyValidationCheck(MeshComp))
	{
		return;
	}
	
	if (MeshComp->GetOwner()->HasAuthority())
	{
		if (ActiveEffectHandle.IsValid())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
			{
				ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
				ActiveEffectHandle.Invalidate();
			}
		}
	}
}
