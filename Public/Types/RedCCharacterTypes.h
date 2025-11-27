// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RedCCharacterTypes.generated.h"

USTRUCT()
struct FCharacterAdditiveMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FName SocketName = FName();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> StaticMesh = nullptr;
};

USTRUCT()
struct FCharacterMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMesh> CharacterMesh = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> CharacterAnimClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TArray<FCharacterAdditiveMeshInfo> CharacterAdditiveMeshes = TArray<FCharacterAdditiveMeshInfo>();
};

USTRUCT(BlueprintType)
struct FCharacterStatInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float AttackPower = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Critical = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Evasion = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Defense = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MovementSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxStamina = 0.f;
};
