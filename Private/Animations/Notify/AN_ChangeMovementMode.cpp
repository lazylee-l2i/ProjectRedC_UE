// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_ChangeMovementMode.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


void UAN_ChangeMovementMode::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MovementMode == MOVE_None)
	{
		return;
	}

	if (MeshComp->GetOwner())
	{
		if (const ACharacter* Owner = Cast<ACharacter>(MeshComp->GetOwner()))
		{
			Owner->GetCharacterMovement()->SetMovementMode(MovementMode);	
		}
	}
}

FString UAN_ChangeMovementMode::GetNotifyName_Implementation() const
{
	FString Result = FString();
	switch (MovementMode)
	{
		case MOVE_Walking:
			Result = FString("Walking");
			break;
		case MOVE_Flying:
			Result = FString("Flying");
			break;
		case MOVE_Swimming:
			Result = FString("Swimming");
			break;
		case MOVE_Falling:
			Result = FString("Falling");
			break;
		case MOVE_Custom:
			Result = FString("Custom");
			break;
		case MOVE_MAX:
			Result = FString("MAX");
			break;
		default:
			Result = FString("None");
	}
	return Result;
}
