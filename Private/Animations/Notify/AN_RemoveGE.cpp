// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_RemoveGE.h"


void UAN_RemoveGE::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (not NotifyValidationCheck(MeshComp))
	{
		return;
	}
}
