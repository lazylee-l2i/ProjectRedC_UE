// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/RedCProjectileAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Projectiles/RedCProjectile.h"
#include "Types/RedCAbilityTypes.h"
#include "Types/RedCGameplayTags.h"


URedCProjectileAbility::URedCProjectileAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URedCProjectileAbility::SpawnProjectile(const UProjectileSpawnEventObject* EventObject)
{
	if (!EventObject)
	{
		return;
	}
	
	const TSubclassOf<ARedCProjectile> ProjectileClass = EventObject->ProjectileInfo.ProjectileClass;
	if (!IsValid(ProjectileClass))
	{
		return;
	}

	APawn* AvatarActor = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!IsValid(AvatarActor))
	{
		return;
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(EventObject->ProjectileInfo.StartLocation);
	SpawnTransform.SetRotation(EventObject->ProjectileInfo.Rotation.Quaternion());

	if (ARedCProjectile* SpawnedProjectile =
		GetWorld()->SpawnActorDeferred<ARedCProjectile>(ProjectileClass, SpawnTransform, AvatarActor, AvatarActor))
	{
		SpawnedProjectile->InitializeProjectile(EventObject->ProjectileInfo);
		SpawnedProjectile->FinishSpawning(SpawnTransform);
	}
}

bool URedCProjectileAbility::PlayMontageAndWait(const FRedCMontageInfo& MontageInfo)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		return false;
	}

	if (!IsValid(MontageInfo.MontageToPlay))
	{
		return false;
	}

	if (auto* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,
		MontageInfo.TaskInstanceName, MontageInfo.MontageToPlay, 1.f, MontageInfo.StartSection))
	{
		Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageBlendOut);
		Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
		Task->ReadyForActivation();

		AddWatchedTask(Task);
	}
	
	return true;
}

void URedCProjectileAbility::WaitGameplayEvent(const FGameplayTag& EventTag, bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		return;
	}

	if (!EventTag.IsValid())
	{
		return;
	}

	if (auto* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag))
	{
		Task->EventReceived.AddDynamic(this, &ThisClass::OnGameplayEventReceived);
		Task->OnlyTriggerOnce = OnlyTriggerOnce;
		Task->OnlyMatchExact = OnlyMatchExact;
		Task->ReadyForActivation();

		AddWatchedTask(Task);
	}
}

void URedCProjectileAbility::WaitInputRelease(bool TestAlreadyReleased)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		return;
	}

	if (auto* Task = UAbilityTask_WaitInputRelease::WaitInputRelease(this, TestAlreadyReleased))
	{
		Task->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		Task->ReadyForActivation();

		AddWatchedTask(Task);
	}
}

void URedCProjectileAbility::OnGameplayEventReceived(const FGameplayEventData Payload)
{
	if (Payload.EventTag == SharedTags::Event_Shared_ProjectileSpawn)
	{
		if (const auto* Object = Cast<UProjectileSpawnEventObject>(Payload.OptionalObject))
		{
			SpawnProjectile(Object);
		}
	}
}

void URedCProjectileAbility::OnInputReleased(float TimeHeld)
{
	// 자식 클래스에서 구현
}

void URedCProjectileAbility::OnMontageCompleted()
{
	// 자식 클래스에서 구현
}

void URedCProjectileAbility::OnMontageInterrupted()
{
	// 자식 클래스에서 구현
}

void URedCProjectileAbility::OnMontageCancelled()
{
	// 자식 클래스에서 구현
}

void URedCProjectileAbility::OnMontageBlendOut()
{
	// 자식 클래스에서 구현
}
