// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "AI/Spawn/RedC_AISpawnPoint.h"
#include "AI/AICharacter/RedCAICharacter.h"

#include "RedCAISpawnActor.generated.h"



USTRUCT(BlueprintType)
struct FAISpawn
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ARedCAICharacter> AIClass = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 Count = 1;

	UPROPERTY()
	int32 TeamID = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float SpawnDelay = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ARedC_AISpawnPoint*> Waypoints = TArray<ARedC_AISpawnPoint*>();
};

UCLASS()
class PROJECTREDC_API ARedCAISpawnActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARedCAISpawnActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:	

	// 스폰 타이머
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	float WaveRepeatInterval = 10.0f;  //스폰 주기
	
	// 스폰 포인트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TArray<ARedC_AISpawnPoint*> SpawnPointsActors; // 전용 SpawnPoint

	//웨이브 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TArray<FAISpawn> Waves;
	
	//풀링 사이즈
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
    int32 PoolSize;

	bool bPlayerInZone = false;

	// UFUNCTION(BlueprintCallable)
	// void MinionIsDead(ARedCAICharacter* DeadAI);  나중에 필요해지면 다시 살리기
	
	void ReturnAIToPool(ARedCAICharacter* AIChar);   // ai 풀로 가져오기

	UFUNCTION(BlueprintCallable)
	void ToggleSpawner();
	
protected:
	
	// AI 풀 타입별 관리
	TMap<TSubclassOf<ARedCAICharacter>, TArray<ARedCAICharacter*>> AI_Pool;
	int32 CurrentWaveIndex = 0;

	FTimerHandle CleanupTimerHandle;
	
	FTimerHandle WaveRepeatTimerHandle;
	
	void InitializeAIPool();
	void SpawnWave();
	void SpawnAI(TSubclassOf<ARedCAICharacter> AIClass, FVector SpawnLocation, int32 TeamID);
	
	//풀링된 ai 가져오는거
	ARedCAICharacter* GetPooledAI(TSubclassOf<ARedCAICharacter> AIClass, int32 TeamID);
	//새로 만드는 ai
	ARedCAICharacter* CreateNewPooledAI(TSubclassOf<ARedCAICharacter> AIClass, int32 TeamID);
};
