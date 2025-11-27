// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/AN_RedCAttackEvent.h"
#include "AN_SendGrabEventNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_SendGrabEventNotify : public UAN_RedCAttackEvent
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
