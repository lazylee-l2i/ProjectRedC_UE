// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_RotationNotify.h"

#include "Characters/RedCHeroCharacter.h"


void UAN_RotationNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner() == nullptr)
	{
		return;
	}

	if (ARedCHeroCharacter* Owner = Cast<ARedCHeroCharacter>(MeshComp->GetOwner()))
	{
		if (bLockRotation)
		{
			Owner->LockControllerRotation();
		}
		else
		{
			Owner->UnlockControllerRotation();
		}	
	}
}

FString UAN_RotationNotify::GetNotifyName_Implementation() const
{
	return bLockRotation ? TEXT("Lock Rotation") : TEXT("Unlock Rotation");
}
