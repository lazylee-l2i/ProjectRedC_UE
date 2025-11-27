// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/NotifyState/ANS_GrabHitScan.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/RedCCharacter.h"
#include "Types/RedCGameplayTags.h"


void UANS_GrabHitScan::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	bIsGrab = false;
}

void UANS_GrabHitScan::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (not NotifyValidationCheck(MeshComp))
	{
		return;
	}
	if (MeshComp->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}
	
	// To-Do
	if (not bIsGrab)
	{
		const TArray<FHitResult> HitResults = SweepBoxTrace(MeshComp);
		for (const FHitResult& Result : HitResults)
		{
			if (ARedCCharacter* Character = Cast<ARedCCharacter>(Result.GetActor()))
			{
				FGameplayEventData EventData;
                EventData.Target = Result.GetActor();
                UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), SharedTags::Event_Shared_Attack_Grab, EventData);
                bIsGrab = true;
				break;
			}
		}
	}
}

