// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Actor.h"
#include "AI/AICharacter/RedCAI_Tower.h"

#include "RedCAIController.generated.h"


class ARedCAICharacter;
struct FAIStimulus;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API ARedCAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ARedCAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());  

	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	UAIPerceptionComponent* AIPerceptionComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	UAISenseConfig_Sight* SightConfig;
	
	UFUNCTION()
	void InitializeAndActivateAI(int32 TeamID, UBehaviorTree* BT);  

	UFUNCTION()
	void ReRegisterStimuli();
	
	void BindPerceptionDelegates();
	void UnbindPerceptionDelegates();
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
	UFUNCTION()
	void TargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	void TargetPerceptionForgotten(AActor* Actor);

	FGenericTeamId GetTeamIdFromActor(const AActor* Actor) const;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	const UObject* GetCurrentTarget() const;

	void SetCurrentTarget(AActor* NewTarget);

	
private:
	
	UFUNCTION()
	void OnTargetAIDeath(ARedCAICharacter* DeadAI);
	
	void ClearAggroState();
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	FName TargetBlackBoardKeyName;
	
	AActor* GetNextPerceiveActor() const;

	UFUNCTION()
	void OnTargetTowerDestroyed(AActor* DestroyedTower);
	void BindTowerDestroyedIfNeeded(AActor* NewTarget);
	void UnbindTowerDestroyedIfNeeded();

	TWeakObjectPtr<ARedCAI_Tower> BoundTower;

	void BindTargetDeathTag(AActor* NewTarget);
	void UnbindTargetDeathTag();
	void OnTargetDeathTagChanged(const FGameplayTag Tag, int32 NewCount);

	FDelegateHandle TargetDeathTagHandle;
	TWeakObjectPtr<UAbilitySystemComponent> TargetASC;
};
