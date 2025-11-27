// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_SendGrabEventNotify.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/RedCCharacter.h"
#include "Types/RedCGameplayTags.h"


void UAN_SendGrabEventNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (not NotifyValidationCheck(MeshComp))
	{
		
		return;
	}
	if (MeshComp->GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	if (EventTag.MatchesTag(SharedTags::Event_Shared_Attack_Damage))
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), SharedTags::Event_Shared_Attack_Damage, FGameplayEventData());
	}
	else if (EventTag.MatchesTag(SharedTags::Event_Shared_Attack_Grab))
	{
		TArray<FHitResult> HitResults = SweepBoxTrace(MeshComp);
		if (HitResults.Num() > 0)
		{
			FHitResult HitResult = HitResults[0];
			if (Cast<ARedCCharacter>(HitResult.GetActor())->GetAbilitySystemComponent()->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
			{
				return;
			}
			FGameplayEventData EventData;
			EventData.Target = HitResult.GetActor();
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), SharedTags::Event_Shared_Attack_Grab, EventData);
		}
	}
}
