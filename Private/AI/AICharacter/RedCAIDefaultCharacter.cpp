// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter/RedCAIDefaultCharacter.h"

#include "AIController.h"
#include "AI/GAS/RedCAIAbilitySystemComponent.h"
#include "BehaviorTree/Tasks/BTTask_RunBehaviorDynamic.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Types/RedCLogChannels.h"


ARedCAIDefaultCharacter::ARedCAIDefaultCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	IsAlive = false;
	bReplicates = true;
	bNetLoadOnClient = true;
	bAlwaysRelevant = true;

	AIPerceptionStimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComponent"));
	AIPerceptionStimuliSourceComp->bAutoRegister = false;
	AIPerceptionStimuliSourceComp->bAutoActivate = false;

	AbilitySystemComponent = CreateDefaultSubobject<URedCAIAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void ARedCAIDefaultCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (not OnLifeChanged.IsBound())
	{
		OnLifeChanged.AddDynamic(this, &ARedCAIDefaultCharacter::OnReceiveLifeChanged);	
	}
	
	if (HasAuthority())
	{
		// Cast<URedCAIAbilitySystemComponent>(AbilitySystemComponent)->InitializeAbilitiesAndEffectsSafe()
	}
}

void ARedCAIDefaultCharacter::OnReceiveLifeChanged()
{
	IsAlive = (not IsAlive);
	if (IsAlive)
	{
		ActivateAICharacter();
	}
	else
	{
		DeactivateAICharacter();
	}
}

void ARedCAIDefaultCharacter::SetWayPoints(const TArray<AActor*> InPoints)
{
	WayPoints.Empty();
	for (auto& WayPoint : InPoints)
	{
		WayPoints.Add(WayPoint);
	}
	CurrentWayPoint = WayPoints[0];
}

TArray<AActor*> ARedCAIDefaultCharacter::GetWayPoints() const
{
	return WayPoints;
}

bool ARedCAIDefaultCharacter::CurrentIsLast() const
{
	if (CurrentWayPoint == nullptr)
	{
		return true;
	}
	return false;
}

bool ARedCAIDefaultCharacter::CurrentIsFirst() const
{
	if (CurrentWayPoint != nullptr)
	{
		return (WayPoints[0] == CurrentWayPoint);
	}
	return false;
}

void ARedCAIDefaultCharacter::SetCurrentWayPointToNext()
{
	if (AActor* NextPoint = GetNextWayPoint())
	{
		CurrentWayPoint = NextPoint;
	}
	else
	{
		CurrentWayPoint = nullptr;
	}
}

AActor* ARedCAIDefaultCharacter::GetCurrentWayPoint() const
{
	return CurrentWayPoint;
}

AActor* ARedCAIDefaultCharacter::GetNextWayPoint()
{
	bool FoundCurrentPoint = false;
	
	for (auto& WayPoint : WayPoints)
	{
		if (WayPoint == CurrentWayPoint)
		{
			FoundCurrentPoint = true;
			continue;
		}
		if (FoundCurrentPoint)
		{
			return WayPoint;
		}
	}
	return nullptr;
}

void ARedCAIDefaultCharacter::SetFirstBornLocation(FVector Location)
{
	FirstBornLocation = Location;
}

// AI Character 활성화
void ARedCAIDefaultCharacter::ActivateAICharacter()
{
	//SetActorHiddenInGame(false);
	//UE_LOG(LogRedCAI, Log, TEXT("AI Activation!"));
	AIPerceptionStimuliSourceComp->RegisterForSense(UAISense_Sight::StaticClass());
	AIPerceptionStimuliSourceComp->RegisterWithPerceptionSystem();	
	//SetActorEnableCollision(true);
	
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		//UE_LOG(LogRedCAI, Log, TEXT("AI Controller Activation BT!"));
		AIC->RunBehaviorTree(BTClass);
	}
	else
	{
		//UE_LOG(LogRedCAI, Error, TEXT("AI Controller is NULL!"));
	}

}

void ARedCAIDefaultCharacter::DeactivateAICharacter()
{
	//UE_LOG(LogRedCAI, Log, TEXT("AI Deactivation!"));
	//SetActorHiddenInGame(true);
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBehaviorTreeComponent* BTComp = AIC->FindComponentByClass<UBehaviorTreeComponent>())
		{
			BTComp->StopTree(EBTStopMode::Safe);
		}
		AIC->StopMovement();
		AIC->GetBrainComponent()->StopLogic(TEXT("Deactivating AI"));
	}
	AIPerceptionStimuliSourceComp->UnregisterFromPerceptionSystem();
	SetActorLocation(FirstBornLocation);
	//SetActorEnableCollision(false);
}
