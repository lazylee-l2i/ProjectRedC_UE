// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Animations/NotifyState/ANS_RedCNotifyState.h"
#include "ANS_GameplayEffect.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UANS_GameplayEffect : public UANS_RedCNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffect;

	UPROPERTY(EditAnywhere)
	FGameplayTag EffectTag;
	
	FActiveGameplayEffectHandle ActiveEffectHandle;
};
