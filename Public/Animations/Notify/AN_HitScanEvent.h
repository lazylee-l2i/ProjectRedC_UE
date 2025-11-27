// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/AN_RedCAttackEvent.h"
#include "GameplayTagContainer.h"
#include "Types/RedCAbilityTypes.h"
#include "Types/RedCGameplayTags.h"
#include "AN_HitScanEvent.generated.h"

class UDebugDrawComponent;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_HitScanEvent : public UAN_RedCAttackEvent
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	UPROPERTY(EditAnywhere, Category = "Hit Paramters", meta=(Categories="Event.Shared.HitReact"))
	FGameplayTag HitStrengthTag;

	UPROPERTY(EditAnywhere, Category = "Hit Paramters")
	float KnockbackDistance;

	UPROPERTY(EditAnywhere, Category = "Hit Paramters")
	float LaunchHeight;

	UPROPERTY(EditAnywhere, Category = "Hit Paramters")
	float StaggerDuration;

	UPROPERTY(EditAnywhere, Category = "Melee Attack Parameter")
	EWeaponType WeaponType;
};
