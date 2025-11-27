// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/NotifyState/ANS_BaseHitScan.h"
#include "ANS_GrabHitScan.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UANS_GrabHitScan : public UANS_BaseHitScan
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY()
	bool bIsGrab = false;
};
