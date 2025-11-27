// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RedCCharacterSkillTypes.generated.h"

UENUM(BlueprintType)
enum class ECharacterAttackType : uint8
{
	None,
	EAttack,
	Grab,
	Spacebar,
	SRAttack,
	RAttack,
	SLAttack
};

USTRUCT(BlueprintType)
struct FCharacterSkillInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown = 0.f;
};

class ECharacterAttackTypeHelper
{
public:
	static ECharacterAttackType StringToAttackType(const FString& Str)
	{
		if (Str.Equals("EAttack", ESearchCase::IgnoreCase))  return ECharacterAttackType::EAttack;
		if (Str.Equals("Grab", ESearchCase::IgnoreCase))  return ECharacterAttackType::Grab;
		if (Str.Equals("Dodge", ESearchCase::IgnoreCase))  return ECharacterAttackType::Spacebar;
		if (Str.Equals("SRAttack", ESearchCase::IgnoreCase))  return ECharacterAttackType::SRAttack;
		if (Str.Equals("RAttack", ESearchCase::IgnoreCase)) return ECharacterAttackType::RAttack;
		if (Str.Equals("SLAttack", ESearchCase::IgnoreCase))  return ECharacterAttackType::SLAttack;
		return ECharacterAttackType::None;
	};
};

