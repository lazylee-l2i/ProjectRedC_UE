// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/AN_RedCAnimNotify.h"
#include "BaseClass/AN_RedCBattleNotify.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ComboEvent.generated.h"

UCLASS()
class PROJECTREDC_API UAN_ComboEvent : public UAN_RedCBattleNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	virtual FString GetNotifyName_Implementation() const override;
};
