// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_RemoveTag.h"


void UAN_RemoveTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (not NotifyValidationCheck(MeshComp))
	{
		return;
	}

	if (EventTag.IsValid())
	{
		if (GetMeshAbilitySystemComponent(MeshComp)->HasMatchingGameplayTag(EventTag))
		{
			GetMeshAbilitySystemComponent(MeshComp)->RemoveLooseGameplayTag(EventTag);	
		}
		
	}
}
