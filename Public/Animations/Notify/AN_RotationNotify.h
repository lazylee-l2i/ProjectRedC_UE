// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AN_RotationNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAN_RotationNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	virtual FString GetNotifyName_Implementation() const override;
private:
	UPROPERTY(EditAnywhere, Category = "Lock")
	bool bLockRotation = false;
	
	
};
