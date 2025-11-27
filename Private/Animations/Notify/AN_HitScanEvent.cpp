// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_HitScanEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "AI/AICharacter/RedCAI_Tower.h"
#include "Characters/RedCHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/RedCAbilityTypes.h"
#include "Types/RedCLogChannels.h"

void UAN_HitScanEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!NotifyValidationCheck(MeshComp))
	{
		return;
	}

	if (ARedCAICharacter* AI = MeshComp->GetOwner<ARedCAICharacter>())
	{
		if (MeshComp->GetOwnerRole() != ROLE_Authority)
		{
			return;
		}
	}
	else if (ARedCHeroCharacter* Hero = MeshComp->GetOwner<ARedCHeroCharacter>())
	{
		if (MeshComp->GetOwnerRole() != ROLE_AutonomousProxy)
		{
			return;
		}
	}

	FHitReactMessage HitReactMessage;
	HitReactMessage.AttackDirection = MeshComp->GetOwner()->GetActorForwardVector();
	HitReactMessage.GameplayCueTag = GameplayCueTag;
	HitReactMessage.HitStrengthTag = HitStrengthTag;
	HitReactMessage.DamageAmount = DamageAmount;
	HitReactMessage.DamageMultiplier = DamageMultiplier;
	HitReactMessage.KnockbackDistance = KnockbackDistance;
	HitReactMessage.KnockDownHeight = LaunchHeight;
	HitReactMessage.StaggerDuration = StaggerDuration;
	float ValidationDistance = Range.Y + MeshComp->GetSocketTransform(
		TargetSocketName, RTS_Component).GetLocation().Y;
	HitReactMessage.MaximumAttackLength = ValidationDistance * 1.2f;
	
	TArray<FHitResult> HitResults = SweepBoxTrace(MeshComp);
	
	if (HitResults.Num() > 0)
	{
		for (FHitResult& HitResult : HitResults)
		{
			HitReactMessage.WeaponType = WeaponType;
			HitReactMessage.ImpactLocation = HitResult.ImpactPoint;
			if (ARedCCharacter* Character = Cast<ARedCCharacter>(MeshComp->GetOwner()))
			{
				if (ARedCAI_Tower* Tower = Cast<ARedCAI_Tower>(HitResult.GetActor()))
				{
					Character->GetCombatComponent()->ServerApplyDamageToTarget(HitReactMessage, Tower);
				}
				else
				{
					Character->ServerReceiveHitReactData(HitReactMessage, HitResult.GetActor());	
				}
			}
		}
	}
	
}


FString UAN_HitScanEvent::GetNotifyName_Implementation() const
{
	FString NotifyVisibleName = FString();
	if (HitStrengthTag.IsValid())
	{
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(HitStrengthTag, TagNames);
		if (TagNames.Num() > 0)
		{
			NotifyVisibleName = TagNames.Last().ToString() + " Hit";
			return NotifyVisibleName;
		}
		else
		{
			return FString("Hit");
		}
	}
	else
	{
		return FString("Hit");	
	}
}


