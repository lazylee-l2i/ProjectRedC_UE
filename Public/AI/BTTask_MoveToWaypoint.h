// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/Spawn/RedCAISpawnActor.h"


#include "BTTask_MoveToWaypoint.generated.h"

class ARedCAIController;
class ARedCAICharacter;

/**
 * 
 */


UCLASS()
class PROJECTREDC_API UBTTask_MoveToWaypoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	UBTTask_MoveToWaypoint();
	
	int32 CapturedGenerationId = INDEX_NONE; 

	
protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	

private:
	
	UFUNCTION()
	void CleanupDelegates();
	
	FDelegateHandle MoveFinishedHandle;
	bool bFinishing = false;

	TWeakObjectPtr<UBehaviorTreeComponent>	 OwnerCompWeak;
	TWeakObjectPtr<ARedCAIController>		 AIControllerWeak;
	TWeakObjectPtr<ARedCAICharacter>		 AICharacterWeak;
	TWeakObjectPtr<ARedCAISpawnActor>		 SpawnerWeak;
	
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
        	
	UPROPERTY()
	UBehaviorTreeComponent* OwnerCompPtr;
	UPROPERTY()
	ARedCAIController* AIController;
	UPROPERTY()
	ARedCAICharacter* AICharacter;

	FAIRequestID MoveRequestID;

};
