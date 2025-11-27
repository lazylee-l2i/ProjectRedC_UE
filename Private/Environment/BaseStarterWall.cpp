// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/BaseStarterWall.h"

#include <memory>

#include "Frameworks/GameState/GameStateArena.h"
#include "Net/UnrealNetwork.h"


ABaseStarterWall::ABaseStarterWall()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	SetRootComponent(WallMesh);
}

void ABaseStarterWall::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		if (AGameStateArena* GS = GetWorld()->GetGameState<AGameStateArena>())
		{
			GS->AddStarterWall(this);
		}
	}
}


