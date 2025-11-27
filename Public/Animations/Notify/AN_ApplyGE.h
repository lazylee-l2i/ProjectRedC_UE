// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/Notify/BaseClass/AN_RedCTriggerGE.h"
#include "AN_ApplyGE.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_ApplyGE : public UAN_RedCTriggerGE
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
