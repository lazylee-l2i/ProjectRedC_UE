// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_RedCNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UANS_RedCNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	bool NotifyValidationCheck(const USkeletalMeshComponent* MeshComp);
	bool CheckSameTeam(const USkeletalMeshComponent* MeshComp, AActor* Other) const;
};
