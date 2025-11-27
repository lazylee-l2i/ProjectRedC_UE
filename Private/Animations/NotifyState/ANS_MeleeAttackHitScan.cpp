// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/NotifyState/ANS_MeleeAttackHitScan.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "AI/AICharacter/RedCAI_Tower.h"
#include "Characters/RedCCharacter.h"
#include "Characters/RedCHeroCharacter.h"
#include "Characters/Components/CombatComponent.h"

void UANS_MeleeAttackHitScan::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
    if (not NotifyValidationCheck(MeshComp))
    {
       return;
    }
 
    if (MeshComp->GetOwner<ARedCAICharacter>())
    {
       if (MeshComp->GetOwnerRole() != ROLE_Authority)
       {
          return;
       }
    }
    else if (MeshComp->GetOwner<ARedCHeroCharacter>())
    {
       if (MeshComp->GetOwnerRole() != ROLE_AutonomousProxy)
       {
          return;
       }
    }
    
    // To-Do
    const TArray<FHitResult> HitResults = SweepBoxTrace(MeshComp);
    if (HitResults.Num() == 0)
    {
       return;
    }
	
	FHitReactMessage HitReactMessage;
	HitReactMessage.AttackDirection = MeshComp->GetOwner()->GetActorForwardVector();
	HitReactMessage.DamageAmount = DamageAmount;
	HitReactMessage.DamageMultiplier = DamageMultiplier;
	HitReactMessage.HitStrengthTag = HitStrengthTag;
	HitReactMessage.KnockbackDistance = KnockbackDistance;
	HitReactMessage.KnockDownHeight = LaunchHeight;
	HitReactMessage.StaggerDuration = StaggerDuration;
	HitReactMessage.GameplayCueTag = GameplayCueTag;
	
	const float ValidationDistance = AttackRange.Y + MeshComp->GetSocketTransform(
	TargetSocketName, RTS_Component).GetLocation().Y;
	HitReactMessage.MaximumAttackLength = ValidationDistance * 1.2f;

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (not HitActor)
		{
			continue;
		}

		const bool bAlreadyHit = HitActors.ContainsByPredicate([HitActor](const TWeakObjectPtr<AActor>& WeakActor)
		{
			return WeakActor.IsValid() && WeakActor.Get() == HitActor;	
		});

		if (bAlreadyHit)
		{
			continue;
		}

		HitActors.Add(HitActor);
		HitReactMessage.WeaponType = WeaponType;
		HitReactMessage.ImpactLocation = HitResult.ImpactPoint;
		if (ARedCCharacter* Character = Cast<ARedCCharacter>(MeshComp->GetOwner()))
		{
			if (ARedCAI_Tower* Tower = Cast<ARedCAI_Tower>(HitActor))
			{
				Character->GetCombatComponent()->ServerApplyDamageToTarget(HitReactMessage, Tower);
			}
			else
			{
				Character->ServerReceiveHitReactData(HitReactMessage, HitActor);
			}
			
		}
	}
}


FString UANS_MeleeAttackHitScan::GetNotifyName_Implementation() const
{
	if (not HitStrengthTag.IsValid())
	{
		return FString(TEXT("Hit"));
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(HitStrengthTag, TagNames);
    
	return TagNames.Num() > 0 
		? FString::Printf(TEXT("%s Hit"), *TagNames.Last().ToString())
		: FString(TEXT("Hit"));
}