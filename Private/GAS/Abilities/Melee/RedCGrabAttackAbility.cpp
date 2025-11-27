// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Melee/RedCGrabAttackAbility.h"

#include "ContextualAnimSceneActorComponent.h"
#include "ContextualAnimSceneAsset.h"
#include "ContextualAnimUtilities.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/RedCHeroCharacter.h"
#include "Characters/Components/CombatComponent.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"


URedCGrabAttackAbility::URedCGrabAttackAbility()
{
}

bool URedCGrabAttackAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	
	UE_LOG(LogRedCGA, Warning, TEXT("Remain CoolTime : %f"), GetCooldownTimeRemaining(ActorInfo));
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void URedCGrabAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			GrabMontage,
			1.f
		);
		MontageTask->OnCancelled.AddDynamic(this, &URedCGrabAttackAbility::OnEndAbility);
		MontageTask->OnBlendOut.AddDynamic(this, &URedCGrabAttackAbility::OnEndAbility);
		MontageTask->OnCompleted.AddDynamic(this, &URedCGrabAttackAbility::OnEndAbility);
		MontageTask->ReadyForActivation();
		AddWatchedTask(MontageTask);

		// 잡기 종료
		UAbilityTask_WaitGameplayEvent* WaitEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			SharedTags::Event_Call_End.GetTag()
		);
		WaitEndTask->EventReceived.AddDynamic(this, &URedCGrabAttackAbility::OnGrabEnd);
		WaitEndTask->ReadyForActivation();
		AddWatchedTask(WaitEndTask);

		// 잡기 판정
		UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			SharedTags::Event_Shared_Attack_Grab.GetTag()
		);
		WaitTask->EventReceived.AddDynamic(this, &URedCGrabAttackAbility::OnGrabTarget);
		WaitTask->ReadyForActivation();
		AddWatchedTask(WaitTask);
	}

	// Only Server
	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* DamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				SharedTags::Event_Shared_Attack_Damage.GetTag()
			);
		DamageEventTask->EventReceived.AddDynamic(this, &URedCGrabAttackAbility::OnDamageEvent);
		DamageEventTask->ReadyForActivation();
		AddWatchedTask(DamageEventTask);
	}
}

void URedCGrabAttackAbility::OnGrabTarget(FGameplayEventData Payload)
{
	
	//UE_LOG(LogRedCGA, Warning, TEXT("OnGrabTarget : No Grab Target"));
	// 첫번째로 인식된 대상 1개에게만 처형이 발동되야 하므로 처음 결과값만 가져옴.

	/***************************************************
	 *			일단, 유튭에 나온거 그대로 따라해봄
	 ***************************************************/
	if (K2_HasAuthority())
	{
		ARedCHeroCharacter* Attacker = Cast<ARedCHeroCharacter>(GetAvatarActorFromActorInfo());
		GrabTarget = static_cast<AActor*>(Payload.Target);
		Attacker->ServerRunMotionWarping(GrabSceneAsset, static_cast<AActor*>(Payload.Target), GetUpMontage);
	}
	
}

void URedCGrabAttackAbility::OnGrabEnd(FGameplayEventData Payload)
{
	OnEndAbility();
}

void URedCGrabAttackAbility::OnDamageEvent(FGameplayEventData Payload)
{
	if (IsValid(GrabTarget))
	{
		if (ARedCCharacter* Attacker = Cast<ARedCCharacter>(GrabTarget))
		{
			FHitReactMessage HitReactMessage;
			HitReactMessage.DamageAmount = DamageAmount;
			HitReactMessage.DamageMultiplier = DamageMultiplier;
			Attacker->GetCombatComponent()->ServerApplyDamageToOwner(HitReactMessage, GetAvatarActorFromActorInfo());
		}
		//ApplyDamageEffect(GrabTarget, Payload.EventMagnitude);
	}
	//OnEndAbility();
}