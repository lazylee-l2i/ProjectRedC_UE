// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animations/NotifyState/ANS_RedCNotifyState.h"
#include "ANS_BaseHitScan.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UANS_BaseHitScan : public UANS_RedCNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UAbilitySystemComponent* GetMeshAbilitySystemComponent(USkeletalMeshComponent* MeshComp);	

	UPROPERTY(EditAnywhere, Category = "Attack Parameter")
	FName TargetSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Attack Parameter")
	bool DrawDebug;
	
	UPROPERTY(EditAnywhere, Category = "Attack Parameter")
	FVector AttackRange;
	
	UPROPERTY(EditAnywhere, Category = "Attack Parameter")
	float DamageAmount;

	UPROPERTY(EditAnywhere, Category = "Attack Parameter")
	float DamageMultiplier;
	
	UPROPERTY(EditAnywhere, Category = "GameplayCue Tag")
	FGameplayTag GameplayCueTag;

	TArray<FHitResult> SweepBoxTrace(const USkeletalMeshComponent* MeshComp) const;

	TArray<TWeakObjectPtr<AActor>> HitActors;
	
private:
	mutable TArray<TEnumAsByte<EObjectTypeQuery>> CachedObjectTypes;
	mutable bool bObjectTypesCached = false;
};
