// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/RedCAbilityTypes.h"
#include "RespawnManagerActor.generated.h"


UCLASS()
class PROJECTREDC_API ARespawnManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARespawnManagerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerID")
	int32 PlayerID;

	FVector GetSpawnLocation() const { return GetActorLocation(); }
	FRotator GetSpawnRotation() const { return GetActorRotation(); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeamType TeamType;
	
};
