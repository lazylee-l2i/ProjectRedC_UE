// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedCAISpawner.generated.h"

class ARedCAIDefaultCharacter;
class UBehaviorTree;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnerActivate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnerDeactivate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAICharacterDead, ARedCAIDefaultCharacter*, AICharacter);

UCLASS()
class PROJECTREDC_API ARedCAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARedCAISpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool ValidateAIClassAndWaypoints() const;
	
#pragma region Spawner Parameters
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner", meta = (ToolTip = "AI Character Class"))
	TSubclassOf<ACharacter> AIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner", meta = (ToolTip = "WayPoint Container"))
	TArray<TObjectPtr<AActor>> Waypoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner", meta = (ToolTip = "Pool Tank Location(Actor)"))
	TObjectPtr<AActor> PoolTankLocationActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner", meta = (ToolTip = "Spawn Location", MakeEditWidget = true))
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner", meta = (ToolTip = "Team ID"))
	int32 TeamID = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner", meta = (ClampMin = "1", ClampMax = "3", ToolTip = "Wave Size. 1 ~ 3"))
	int32 BatchSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner", meta = (ClampMin = "1", ClampMax = "100" , ToolTip = "AI Pool Size. 1 ~ 100"))
	int32 PoolSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner", meta = (ClampMin = "1", ClampMax = "120", ToolTip = "Wave Repeat Interval Time. 1 ~ 120"))
	float WaveRepeatInterval = 10.0f;
#pragma endregion

#pragma region Spawner Variables
private:
	bool bIsActivate = false;
	
	int32 PoolIndex = 0;

	UPROPERTY()
	TArray<TObjectPtr<ARedCAIDefaultCharacter>> SpawnedActors;
	
	FTimerHandle SpawnTimerHandle;

public:
	UPROPERTY(BlueprintCallable)
	FSpawnerActivate OnActivateSpawnerDelegate;

	UPROPERTY(BlueprintCallable)
	FSpawnerDeactivate OnDeactivateSpawnerDelegate;

	UPROPERTY(BlueprintCallable)
	FAICharacterDead OnAICharacterDeadDelegate;
#pragma endregion

#pragma region Spawner Functions
protected:
	void InitializeSpawner();

	UFUNCTION(BlueprintCallable)
	ARedCAIDefaultCharacter* FindAvailableIndexAndGetThatActor();

	UFUNCTION()
	void SpawnAI();

	UFUNCTION()
	void OnActivateSpawner();

	UFUNCTION()
	void OnDeactivateSpawner();

	UFUNCTION()
	void OnAICharacterDead(ARedCAIDefaultCharacter* AICharacter);

public:
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetWaypoints() const;
	
#pragma endregion
	
};
