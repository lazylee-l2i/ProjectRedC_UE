// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/RedCAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/PawnMovementComponent.h"


void URedCAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	NativeUpdateSpeedAndDirection(DeltaSeconds);
}

void URedCAnimInstance::NativeUpdateSpeedAndDirection(float DeltaSeconds)
{
	const APawn* Pawn = TryGetPawnOwner();
	if (not Pawn)
	{
		return;
	}

	const FVector Velocity = Pawn->GetVelocity();
	const FRotator Rotation = Pawn->GetActorRotation();

	Speed = Velocity.Size2D();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Rotation);
}

bool URedCAnimInstance::IsOnGround() const
{
	const APawn* Pawn = TryGetPawnOwner();
	if (not Pawn)
	{
		return false;
	}
	
	return Pawn->GetMovementComponent()->IsMovingOnGround();
}

bool URedCAnimInstance::IsFalling() const
{
	const APawn* Pawn = TryGetPawnOwner();
	if (not Pawn)
	{
		return false;
	}
	return Pawn->GetMovementComponent()->IsFalling();
}
