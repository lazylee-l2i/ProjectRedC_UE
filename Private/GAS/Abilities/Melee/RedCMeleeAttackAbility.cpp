// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Melee/RedCMeleeAttackAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GameplayTagsManager.h"
#include "Characters/RedCCharacter.h"
#include "GAS/Statics/RedCAbilitySystemStatics.h"
#include "Types/RedCAbilityTypes.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"


#pragma region Ability Setup
URedCMeleeAttackAbility::URedCMeleeAttackAbility()
{
	AbilityTags.AddTag(PlayerAbilityTags::Player_Ability_Attack_LAttack);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	BlockAbilitiesWithTag.AddTag(PlayerAbilityTags::Player_Ability_Attack_LAttack);
	WaitComboInputPressTask = nullptr;
}

void URedCMeleeAttackAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle		Handle,
	const FGameplayAbilityActorInfo*		ActorInfo,
	const FGameplayAbilityActivationInfo	ActivationInfo,
	const FGameplayEventData*				TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		SetupMeleeAttackTasks();
		SetupWaitComboInputPress();
	}
}

#pragma region Setup Tasks
void URedCMeleeAttackAbility::SetupWaitComboInputPress()
{
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		if (IsValid(WaitComboInputPressTask))
		{
			WaitComboInputPressTask->EndTask();
			WaitComboInputPressTask = nullptr;
		}
		UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
		WaitInputPress->OnPress.AddDynamic(this, &URedCMeleeAttackAbility::HandleInputPress);
		WaitInputPress->ReadyForActivation();
		WaitComboInputPressTask = WaitInputPress;
		AddWatchedTask(WaitInputPress);
	}
	
}

void URedCMeleeAttackAbility::OnEndAbility()
{
	RemoveMotionWarpingPoint();
	Super::OnEndAbility();
}

void URedCMeleeAttackAbility::SetupMeleeAttackTasks()
{
	// 근거리 공격에 대한 PlayMontage Task 설정
	AddMotionWarpingPoint();
	UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage, 1.f, FName("Combo1"));
	PlayTask->OnBlendOut.AddDynamic(this, &URedCMeleeAttackAbility::OnEndAbility);
	PlayTask->OnCancelled.AddDynamic(this, &URedCMeleeAttackAbility::OnEndAbility);
	PlayTask->OnInterrupted.AddDynamic(this, &URedCMeleeAttackAbility::OnEndAbility);
	PlayTask->OnCompleted.AddDynamic(this, &URedCMeleeAttackAbility::OnEndAbility);
	PlayTask->ReadyForActivation();
	AddWatchedTask(PlayTask);

	
	
	UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetComboChangedEventTag(), nullptr, false, false);
	WaitTask->EventReceived.AddDynamic(this, &URedCMeleeAttackAbility::ComboChangedEventReceived);
	WaitTask->ReadyForActivation();
	AddWatchedTask(WaitTask);
}
#pragma endregion

#pragma endregion

#pragma region Combo Handle
// 솔직히 아래 static들은 왜 쓰는지 모르겠지만, 추후 Tag의 변화가 이뤄지면 발생할 수 도 있는 에러 방지를 위해서는 좋을수도?
FGameplayTag URedCMeleeAttackAbility::GetComboChangedEventEndTag()
{
	return SharedTags::Event_Shared_Change_End.GetTag();
}

FGameplayTag URedCMeleeAttackAbility::GetComboTargetEventTag()
{
	return SharedTags::Event_Shared_Attack_Damage.GetTag();
}

FGameplayTag URedCMeleeAttackAbility::GetComboChangedEventTag()
{
	return SharedTags::Event_Shared_Change.GetTag();
}

// 다음 콤보 실행이 이뤄지면 다음 콤보에 대한 새로운 Task를 생성하고 몽타주 섹션을 변경함.
void URedCMeleeAttackAbility::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();
	TryCommitCombo();
}

// 입력 Task를 통해 실행되면 NextCombo에 따라 다음 콤보를 실행
void URedCMeleeAttackAbility::TryCommitCombo()
{
	if (NextComboName == NAME_None or NextComboName == FName("End"))
	{
		return;
	}

	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (not OwnerAnimInst)
	{
		return;
	}

	if (OwnerAnimInst->Montage_GetCurrentSection(ComboMontage) == NextComboName)
	{
		return;
	}
	//UE_LOG(LogGA_LAttack, Warning, TEXT("Try Commit Combo : %s"), *NextComboName.ToString());
	AddMotionWarpingPoint();
	// Jump To Section 메소드를 활용하여 다음 Combo로 바로 실행
	OwnerAnimInst->Montage_JumpToSection(NextComboName, ComboMontage);
}

void URedCMeleeAttackAbility::AddMotionWarpingPoint()
{
	if (ComboSKillLength.IsEmpty())
	{
		return;
	}
	
	if (IMotionWarpingInterface* MWInterface = Cast<IMotionWarpingInterface>(GetAvatarActorFromActorInfo()))
	{
		uint8 MovementState =  MWInterface->GetCurrentMovementEffect();
		//UE_LOG(LogGA_LAttack, Warning, TEXT("Add Motion Warping Point MovementState : %d"), MovementState);
		float DistanceMultiplier = 0.f;
		switch (MovementState)
		{
		case 0:
			DistanceMultiplier = 1.0f;
			break;
		case 1:
			DistanceMultiplier = 1.5f;
			break;
		case 2:
			DistanceMultiplier = 0.1f;
			break;
		default:
			DistanceMultiplier = 1.f;
			break;
		}
		//UE_LOG(LogRedC, Warning, TEXT("Distance Multiplier : %f, Movement State : %d"), DistanceMultiplier, MovementState);
		FMotionWarpingParam Param = GenerateMotionWarpingParamByCurrentCombo(DistanceMultiplier);
		MWInterface->RemoveAllMotionWarpingTarget();
		
		MWInterface->SetMotionWarpingTarget(Param);
	}
}

void URedCMeleeAttackAbility::RemoveMotionWarpingPoint()
{
	if (IMotionWarpingInterface* MWInterface = Cast<IMotionWarpingInterface>(GetAvatarActorFromActorInfo()))
	{
		MWInterface->RemoveAllMotionWarpingTarget();
	}
}

FMotionWarpingParam URedCMeleeAttackAbility::GenerateMotionWarpingParamByCurrentCombo(float& DistanceMultiplier)
{
	FName CurrentComboName = NextComboName;
	FRotator SKillTargetRotation = GetAvatarActorFromActorInfo()->GetActorRotation();
	FVector SKillTargetPoint = FVector::ZeroVector;
	
	if (CurrentComboName == NAME_None)
	{
		CurrentComboName = "Combo1";
		//float SkillLength = *ComboSKillLength.Find("Combo1") * DistanceMultiplier;
		//SKillTargetPoint = GetAvatarActorFromActorInfo()->GetActorLocation() + GetAvatarActorFromActorInfo()->GetActorForwardVector() * DistanceMultiplier;
	}
	//UE_LOG(LogRedC, Warning, TEXT("CurrentComboName : %s"), *CurrentComboName.ToString());
	float SkillLength = *ComboSKillLength.Find(CurrentComboName) * DistanceMultiplier;
	
	SKillTargetPoint = GetAvatarActorFromActorInfo()->GetActorLocation() + GetActorInfo().PlayerController->GetControlRotation().Vector() * SkillLength;
	
	FMotionWarpingParam Param(
			CurrentComboName,
			SKillTargetPoint,
			SKillTargetRotation
	);
	return Param;
}


// Notify를 통해 Event를 받아서 NextCombo를 설정
// GameplayEvent Task를 위에서 생성하였으므로 Notify만 실행되면 알아서 실행됨.
void URedCMeleeAttackAbility::ComboChangedEventReceived(FGameplayEventData Payload)
{
	FGameplayTag EventTag = Payload.EventTag;

	if (EventTag == GetComboChangedEventEndTag())
	{
		NextComboName = NAME_None;
		return;	
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	if (NextComboName == TagNames.Last())
	{
		return;
	}
	
	NextComboName = TagNames.Last();
}
#pragma endregion
