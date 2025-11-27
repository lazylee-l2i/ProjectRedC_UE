// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/RedCGameplayAbility.h"

#include "Types/RedCGameplayTags.h"

URedCGameplayAbility::URedCGameplayAbility()
{
	ActivationBlockedTags.AddTag(PlayerStateTags::Player_State_Death);
}

UAnimInstance* URedCGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComp)
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}
	return nullptr;
}

void URedCGameplayAbility::EraseRemainTasks()
{
	if (RemainTasks.Num() <= 0)
	{
		return;
	}
	
	for (TObjectPtr<UGameplayTask> Task : RemainTasks)
	{
		if (IsValid(Task))
		{
			Task->EndTask();
		}
	}
	RemainTasks.Empty();
}

void URedCGameplayAbility::AddWatchedTask(UGameplayTask* Task)
{
	if (IsValid(Task))
	{
		RemainTasks.Add(Task);	
	}
}

void URedCGameplayAbility::OnEndAbility()
{
	EraseRemainTasks();
	K2_EndAbility();
}
