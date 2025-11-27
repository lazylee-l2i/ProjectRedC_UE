// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/React/RedCHitReactAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionJumpForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/RedCHeroCharacter.h"
#include "Curves/CurveVector.h"
#include "GameFramework/RootMotionSource.h"
#include "Types/RedCLogChannels.h"


URedCHitReactAbility::URedCHitReactAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void URedCHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{
		if (IRedCCombatSystem* CharacterCombatSystem = Cast<IRedCCombatSystem>(GetAvatarActorFromActorInfo()))
		{
			if (UCombatComponent* CombatComponent = Cast<UCombatComponent>(CharacterCombatSystem->GetCombatComponent()))
			{
				//UE_LOG(LogRedCGA, Log, TEXT("9. Hit React Ability Activate"));
				HitReactMessage = CombatComponent->PopHitMessage();
			}
		}
	}
}

void URedCHitReactAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	K2_EndAbility();
}

void URedCHitReactAbility::SetTriggerTagParameter(const FGameplayTag& TriggerTag)
{
	checkf(TriggerTag.IsValid(), TEXT("Ability %s TriggerTag is not valid."), *GetName());
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = TriggerTag;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

	AbilityTriggers.Add(TriggerData);
}

#pragma region Hit React Variable
UAnimMontage* URedCHitReactAbility::GetHitReactMontage() const
{
	if (IsValid(HitReactMontage))
		return HitReactMontage;

	return nullptr;
}

const FHitReactMessage& URedCHitReactAbility::GetHitData() const
{
	return HitReactMessage;
}
#pragma endregion

#pragma region Hit Data Method
FVector URedCHitReactAbility::GetHitResultLocation() const
{
	FVector HitLocation = GetAvatarActorFromActorInfo()->GetActorLocation() +
		(HitReactMessage.AttackDirection * HitReactMessage.KnockbackDistance);
	return HitLocation;
}

#pragma endregion

#pragma region Task Maker
UAbilityTask_ApplyRootMotionMoveToForce* URedCHitReactAbility::GetApplyRootMotionMoveToTask()
{
	FVector FinalLocation = GetHitResultLocation();
	float Duration = GetHitData().StaggerDuration * 0.5f;
	//UE_LOG(LogRedCGA, Log, TEXT("MoveToLocation Task Create."))


	UAbilityTask_ApplyRootMotionMoveToForce* MoveTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
		this,
		NAME_None,
		FinalLocation,
		Duration,
		true,
		MOVE_Walking,
		true,
		nullptr,
		ERootMotionFinishVelocityMode::ClampVelocity,
		FVector::ZeroVector,
		0.1f
	);
	AddWatchedTask(MoveTask);
	return MoveTask;
}

UAbilityTask_PlayMontageAndWait* URedCHitReactAbility::GetMontageTask()
{
	//UE_LOG(LogRedCGA, Log, TEXT("PlayMontageAndWait Task Create."))
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	NAME_None,
	HitReactMontage,
	1,
	FName(TEXT("Start"))
	);
	AddWatchedTask(MontageTask);
	return MontageTask;
	
}


#pragma endregion
