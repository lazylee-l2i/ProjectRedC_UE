// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/Notify/BaseClass/AN_RedCAttackEvent.h"
#include "AN_ProjectileSpawnEvent.generated.h"

class UProjectileSpawnEventObject;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_ProjectileSpawnEvent : public UAN_RedCBattleNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARedCProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere)
	FName SocketName;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1.f))
	float Speed;
	
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1.f))
	float Distance;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.f))
	float Damage;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.01f))
	float DamageMultiplier;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.f))
	float KnockbackDistance;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.f))
	float LaunchHeight;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.f))
	float StaggerDuration;

	UPROPERTY(EditAnywhere, Meta = (Categories = "GameplayCue"))
	FGameplayTag GameplayCueTag;

	UPROPERTY(EditAnywhere, Meta = (Categories = "Event.Shared.HitReact"))
	FGameplayTag HitStrengthTag;
};
