// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Spawn/RedC_JungleMobSpawnPoint.h"

#include "Net/UnrealNetwork.h"


ARedC_JungleMobSpawnPoint::ARedC_JungleMobSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ARedC_JungleMobSpawnPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARedC_JungleMobSpawnPoint, MobClass);
	DOREPLIFETIME(ARedC_JungleMobSpawnPoint, RespawnTime);
	DOREPLIFETIME(ARedC_JungleMobSpawnPoint, TeamID);
}

