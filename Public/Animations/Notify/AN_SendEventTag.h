// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/AN_RedCBattleNotify.h"
#include "AN_SendEventTag.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_SendEventTag : public UAN_RedCBattleNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
