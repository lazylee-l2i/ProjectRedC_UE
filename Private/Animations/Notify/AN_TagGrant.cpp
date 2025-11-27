// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_TagGrant.h"


void UAN_TagGrant::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (not NotifyValidationCheck(MeshComp))
	{
		return;
	}
	
	if (EventTag.IsValid())
	{
		if (not GetMeshAbilitySystemComponent(MeshComp)->HasMatchingGameplayTag(EventTag))
		{
			GetMeshAbilitySystemComponent(MeshComp)->AddLooseGameplayTag(EventTag);	
		}
	}
}
