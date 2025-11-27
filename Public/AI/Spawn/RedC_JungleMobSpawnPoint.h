// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedC_JungleMobSpawnPoint.generated.h"

UCLASS()
class PROJECTREDC_API ARedC_JungleMobSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ARedC_JungleMobSpawnPoint();

	// 스폰할 몹 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated, Category="Spawn")
	TSubclassOf<ACharacter> MobClass;

	// 리젠 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated, Category="Spawn")
	float RespawnTime = 30.f;

	// 팀 ID (선택)
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated, Category="Spawn")
	int32 TeamID = 0;
	
	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//이미 소환된 몹
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn")
	ACharacter* CurrentMob = nullptr;
};
