// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/React/LightHitReactAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/RedCHeroCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"
#include "GameplayTagContainer.h"

ULightHitReactAbility::ULightHitReactAbility()
{
	SetTriggerTagParameter(FGameplayTag::RequestGameplayTag("Event.Shared.HitReact.Light"));

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void ULightHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (not K2_CommitAbility())
	{
		K2_EndAbility();
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (GetAvatarActorFromActorInfo()->HasAuthority())
		{
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
			{
				// 경직에 따른 위치이동 관련 Task
				UAbilityTask_ApplyRootMotionMoveToForce* MoveTask = GetApplyRootMotionMoveToTask();
				MoveTask->ReadyForActivation();
			}
			
			// 넉백 회복
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, this, &ULightHitReactAbility::BackToRecovery, GetHitData().StaggerDuration
			);
		}
		
		// 경직모션 관련 Task
		UAbilityTask_PlayMontageAndWait* PlayTask = GetMontageTask();
		PlayTask->ReadyForActivation();
	}
}

void ULightHitReactAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	//BackToRecovery();
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void ULightHitReactAbility::BackToRecovery()
{
	if (ARedCCharacter* OwnerCharacter = Cast<ARedCCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (not OwnerCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(PlayerStateTags::Player_State_Combat_KnockedDown) &&
			not OwnerCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
		{
			OwnerCharacter->ServerStopAllMontage(0.35f);
		}
	}
	OnEndAbility();
}



