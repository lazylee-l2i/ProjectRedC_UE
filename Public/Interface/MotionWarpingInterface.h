// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "MotionWarpingInterface.generated.h"

class UMotionWarpingComponent;

USTRUCT()
struct FMotionWarpingParam
{
	GENERATED_BODY()

	UPROPERTY()
	FName WarpingName = NAME_None;
	UPROPERTY()
	FVector WarpingLocation = FVector::ZeroVector;
	UPROPERTY()
	FRotator WarpingRotation = FRotator::ZeroRotator;

	FMotionWarpingParam() = default;
	FMotionWarpingParam(FName InWarpingName, FVector InWarpingLocation, FRotator InWarpingRotation)
		: WarpingName(InWarpingName), WarpingLocation(InWarpingLocation), WarpingRotation(InWarpingRotation)
	{
	}
	FMotionWarpingParam(const FMotionWarpingParam& Other)
		: WarpingName(Other.WarpingName), WarpingLocation(Other.WarpingLocation), WarpingRotation(Other.WarpingRotation)
	{
	}
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMotionWarpingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTREDC_API IMotionWarpingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UMotionWarpingComponent* GetMotionWarpingComponent() const = 0;
	virtual void SetMotionWarpingTarget(FMotionWarpingParam& MotionWarpingParam) = 0;
	virtual void RemoveAllMotionWarpingTarget() = 0;
	virtual uint8 GetCurrentMovementEffect() = 0;
};
