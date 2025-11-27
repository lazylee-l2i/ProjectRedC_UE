// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RedCCharacter.h"
#include "RedCAIDefaultCharacter.generated.h"

class UBehaviorTree;
class AAIController;
class UAIPerceptionStimuliSourceComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotifyAILifeChanged);

UCLASS()
class PROJECTREDC_API ARedCAIDefaultCharacter : public ARedCCharacter
{
	GENERATED_BODY()
	
	ARedCAIDefaultCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;

// Components
protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComp;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BTClass;
	
#pragma region LifeCycleEvent
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool IsAlive = false;

protected:
	UFUNCTION(BlueprintCallable)
	void ActivateAICharacter();

	UFUNCTION(BlueprintCallable)
	void DeactivateAICharacter();
	
	UFUNCTION()
	void OnReceiveLifeChanged();

public:
	UFUNCTION()
	bool GetIsAlive() const {return IsAlive;}

	UPROPERTY(BlueprintCallable)
	FNotifyAILifeChanged OnLifeChanged;
#pragma endregion

private:
	UPROPERTY()
	AActor* CurrentWayPoint = nullptr;

	UPROPERTY()
	FVector FirstBornLocation;

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> WayPoints;

public:
	UFUNCTION(BlueprintCallable)
	void SetWayPoints(TArray<AActor*> InPoints);

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetWayPoints() const;
	
	UFUNCTION(BlueprintCallable)
	bool CurrentIsLast() const;

	UFUNCTION(BlueprintCallable)
	bool CurrentIsFirst() const;

	UFUNCTION(BlueprintCallable)
	void SetCurrentWayPointToNext();

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentWayPoint() const;
	
	UFUNCTION(BlueprintCallable)
	AActor* GetNextWayPoint();

	UFUNCTION(BlueprintCallable)
	void SetFirstBornLocation(FVector Location);
};
