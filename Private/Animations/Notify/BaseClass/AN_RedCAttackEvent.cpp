// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/BaseClass/AN_RedCAttackEvent.h"

#include "Characters/RedCHeroCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/RedCLogChannels.h"


void UAN_RedCAttackEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
}

TArray<FHitResult> UAN_RedCAttackEvent::SweepBoxTrace(const USkeletalMeshComponent* MeshComp) const
{
	TArray<FHitResult> HitResults;
	TArray<FHitResult> SweepResults;
	
	FVector ImpactPoint = MeshComp->GetSocketLocation(TargetSocketName);

	float HalfX = Range.X * 0.5f;
	float HalfY = Range.Y * 0.5f;
	float HalfZ = Range.Z * 0.5f;
	FVector BoxStart = ImpactPoint - (MeshComp->GetOwner()->GetActorForwardVector() * HalfX);
	FVector BoxEnd = ImpactPoint + (MeshComp->GetOwner()->GetActorForwardVector() * HalfX);

	FVector BoxHalfSize = FVector(0.f, HalfY, HalfZ);
	FRotator BoxOrientation = MeshComp->GetOwner()->GetActorRotation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel4));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MeshComp->GetOwner());

	EDrawDebugTrace::Type DrawDebugTrace = DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(
		MeshComp->GetOwner()->GetWorld(),
		BoxStart, BoxEnd, BoxHalfSize, BoxOrientation,
		ObjectTypes,false, ActorsToIgnore,
		DrawDebugTrace,
		HitResults,
		false
	);

	if (bHit)
	{
		TSet<AActor*> HitActors;
		for (const FHitResult& Result : HitResults)
		{
			if (HitActors.Contains(Result.GetActor()))
			{
				continue;
			}
			if (CheckSameTeam(MeshComp, Result.GetActor()))
			{
				continue;
			}

			if (ARedCHeroCharacter* Player = Cast<ARedCHeroCharacter>(Result.GetActor()))
			{
				if (Player->IsInvincible())
				{
					continue;
				}
			}
			HitActors.Add(Result.GetActor());
			SweepResults.Add(Result);
		}
	}

	return SweepResults;
}
