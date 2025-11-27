// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/NotifyState/ANS_BaseHitScan.h"
#include "Types/RedCAbilityTypes.h"
#include "ANS_MeleeAttackHitScan.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UANS_MeleeAttackHitScan : public UANS_BaseHitScan
{
	GENERATED_BODY()
public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
	virtual FString GetNotifyName_Implementation() const override;

private:
	UPROPERTY(EditAnywhere, Category = "Melee Attack Parameter")
	FGameplayTag HitStrengthTag;
	
	UPROPERTY(EditAnywhere, Category = "Melee Attack Parameter")
	float KnockbackDistance;

	UPROPERTY(EditAnywhere, Category = "Melee Attack Parameter")
	float LaunchHeight;

	UPROPERTY(EditAnywhere, Category = "Melee Attack Parameter")
	float StaggerDuration;
	
	UPROPERTY(EditAnywhere, Category = "Melee Attack Parameter")
	EWeaponType WeaponType;
};
