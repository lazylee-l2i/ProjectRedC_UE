// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Spawn/RedCAISpawnActor.h"
#include "AI/Spawn/RedC_JungleMobSpawnPoint.h"
#include "JungleAISpawnActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API AJungleAISpawnActor : public ARedCAISpawnActor
{
	GENERATED_BODY()
	
	
	public:
	AJungleAISpawnActor();
	
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
	private:
	void SpawnAtPoint(ARedC_JungleMobSpawnPoint* SpawnPoint, int32 TeamID);

	
	// 맵에서 수집한 SpawnPoint 배열
	UPROPERTY()
	TArray<ARedC_JungleMobSpawnPoint*> JungleSpawnPoints;

	// SpawnPoint별 타이머 관리
	TArray<FTimerHandle> SpawnTimerHandles;
};
