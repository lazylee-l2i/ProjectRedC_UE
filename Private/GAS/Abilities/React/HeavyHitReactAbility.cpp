// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/React/HeavyHitReactAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/RedCCharacter.h"
#include "GameFramework/Character.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"

UHeavyHitReactAbility::UHeavyHitReactAbility()
{
	SetTriggerTagParameter(SharedTags::Event_Shared_HitReact_Heavy.GetTag());

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UHeavyHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		if (ARedCCharacter* OwnerCharacter = Cast<ARedCCharacter>(GetAvatarActorFromActorInfo()))
		{
			// Project에서 설정한 중력값 가져옴
			float Gravity = FMath::Abs(GetWorld()->GetGravityZ()); // Project Setting에서 설정한 중력값 가져옴
			float ZVelocity = FMath::Sqrt(2.f * 2.f * Gravity * GetHitData().KnockDownHeight);
			
			FVector LaunchVelocity = FVector::UpVector * ZVelocity +
										GetHitData().AttackDirection * GetHitData().KnockbackDistance;
			
			FRotator LookAtAttackerRotation = GetHitData().AttackDirection.Rotation() +
												FRotator(0.f, 180.f, 0.f);
			
			if (OwnerCharacter->HasAuthority())
			{
				OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);
			}
			
			UAbilityTask_PlayMontageAndWait* PlayTask = GetMontageTask();
			PlayTask->OnInterrupted.AddDynamic(this, &UHeavyHitReactAbility::OnEndAbility);
			PlayTask->OnCancelled.AddDynamic(this, &UHeavyHitReactAbility::OnEndAbility);
			PlayTask->OnBlendOut.AddDynamic(this, &UHeavyHitReactAbility::OnEndAbility);
			PlayTask->ReadyForActivation();

			UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				SharedTags::Event_Call_End.GetTag()
			);
			WaitTask->EventReceived.AddDynamic(this, &UHeavyHitReactAbility::OnEventReceived);
			WaitTask->ReadyForActivation();

			if (GetAvatarActorFromActorInfo()->HasAuthority())
			{
				OwnerCharacter->StartGetUpTimer();
			}
		}
	}
}

void UHeavyHitReactAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	if (not GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
	{
		GetOwnerAnimInstance()->StopAllMontages(0.25f);
	}
	
	OnEndAbility();
}

void UHeavyHitReactAbility::OnEventReceived(FGameplayEventData Payload)
{
	OnEndAbility();
}
