// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_RedCAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_RedCAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	bool NotifyValidationCheck(const USkeletalMeshComponent* MeshComp) const;
	bool CheckSameTeam(const USkeletalMeshComponent* MeshComp, AActor* Other) const;

	UAbilitySystemComponent* GetMeshAbilitySystemComponent(const USkeletalMeshComponent* MeshComp) const;
};
