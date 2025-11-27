// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AN_RedCBattleNotify.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_RedCAttackEvent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_RedCAttackEvent : public UAN_RedCBattleNotify
{
	GENERATED_BODY()
	
protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	bool DrawDebug;
	
	UPROPERTY(EditAnywhere, Category = "Damage Parameters")
	FVector Range;
	
	UPROPERTY(EditAnywhere, Category = "Damage Parameters")
	float DamageAmount;

	UPROPERTY(EditAnywhere, Category = "Damage Parameters")
	float DamageMultiplier;
	
	UPROPERTY(EditAnywhere, Category = "Damage Parameters")
	FName TargetSocketName;

	UPROPERTY(EditAnywhere, Category = "GameplayCue Tag")
	FGameplayTag GameplayCueTag;

	TArray<FHitResult> SweepBoxTrace(const USkeletalMeshComponent* MeshComp) const;
	
};
