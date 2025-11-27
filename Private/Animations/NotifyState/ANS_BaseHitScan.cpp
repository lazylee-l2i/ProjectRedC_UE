// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/NotifyState/ANS_BaseHitScan.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "Characters/RedCHeroCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/RedCGameplayTags.h"


void UANS_BaseHitScan::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (not NotifyValidationCheck(MeshComp))
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

	// To-Do
	HitActors.Reset();
}

void UANS_BaseHitScan::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (not NotifyValidationCheck(MeshComp))
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
	
	// To-Do
	HitActors.Reset();
	
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

UAbilitySystemComponent* UANS_BaseHitScan::GetMeshAbilitySystemComponent(USkeletalMeshComponent* MeshComp)
{
	if (NotifyValidationCheck(MeshComp))
	{
		return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());	
	}
	return nullptr;
}

TArray<FHitResult> UANS_BaseHitScan::SweepBoxTrace(const USkeletalMeshComponent* MeshComp) const
{
	

	if (not bObjectTypesCached)
	{
		CachedObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel4));
		bObjectTypesCached = true;
	}
	
	const FVector ImpactPoint = MeshComp->GetSocketLocation(TargetSocketName);

	const float HalfX = AttackRange.X * 0.5f;
	const float HalfY = AttackRange.Y * 0.5f;
	const float HalfZ = AttackRange.Z * 0.5f;
	const FVector BoxStart = ImpactPoint - (MeshComp->GetOwner()->GetActorForwardVector() * HalfX);
	const FVector BoxEnd = ImpactPoint + (MeshComp->GetOwner()->GetActorForwardVector() * HalfX);

	const FVector BoxHalfSize = FVector(0.f, HalfY, HalfZ);
	const FRotator BoxOrientation = MeshComp->GetOwner()->GetActorRotation();
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Reserve(1);
	ActorsToIgnore.Add(MeshComp->GetOwner());

	const EDrawDebugTrace::Type DrawDebugTrace = DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	TArray<FHitResult> HitResults;
	bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(
		MeshComp->GetOwner()->GetWorld(),
		BoxStart, BoxEnd, BoxHalfSize, BoxOrientation,
		CachedObjectTypes,false, ActorsToIgnore,
		DrawDebugTrace,
		HitResults,
		false
	);

	TArray<FHitResult> FilteredResults;
	if (bHit)
	{
		TSet<AActor*> ProcessedThisFrame;
		for (const FHitResult& Result : HitResults)
		{
			AActor* HitActor = Result.GetActor();
			if (not HitActor)
			{
				continue;
			}
			
			if (ProcessedThisFrame.Contains(HitActor))
			{
				continue;
			}
			
			if (CheckSameTeam(MeshComp, HitActor))
			{
				ProcessedThisFrame.Add(HitActor);
				continue;
			}

			if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
			{
				if (ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death) || ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Invincible))
				{
					ProcessedThisFrame.Add(HitActor);
					continue;
				}
			}
			ProcessedThisFrame.Add(HitActor);
			FilteredResults.Add(Result);
		}
	}

	return FilteredResults;
}
