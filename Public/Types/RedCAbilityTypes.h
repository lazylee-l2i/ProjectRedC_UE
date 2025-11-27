// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/RedCGameplayAbility.h"
#include "RedCAbilityTypes.generated.h"

class ARedCProjectile;
class UGameplayEffect;
class UGameplayAbility;

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Red,
	Blue,
	Neutral,
	None
};

UENUM(BlueprintType)
enum class EActorType : uint8
{
	None,
	Hero,
	Minion,
	Tower
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	Sword,
	Shield,
	Arrow,
	Energy,
	Metal
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	None,
	Leather,
	Plate
};

USTRUCT()
struct FRedCAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
};

USTRUCT()
struct FRedCEffectInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 EffectLevel = 1;
};

USTRUCT()
struct FHitReactMessage
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantizeNormal AttackDirection = FVector::ZeroVector;
	
	UPROPERTY()
	FVector_NetQuantize ImpactLocation = FVector::ZeroVector;

	UPROPERTY()
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY()
	EArmorType ArmorType = EArmorType::None;

	UPROPERTY()
	FGameplayTag HitStrengthTag = FGameplayTag::EmptyTag;

	// For GameplayCue
	UPROPERTY()
	FGameplayTag GameplayCueTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	float DamageAmount = 0.f;
	
	UPROPERTY()
	float DamageMultiplier = 0.f;
	
	UPROPERTY()
	float KnockbackDistance = 0.f;
	
	UPROPERTY()
	float KnockDownHeight = 0.f;
	
	UPROPERTY()
	float StaggerDuration = 0.f;
	
	UPROPERTY()
	float MaximumAttackLength = 1.f;
	
	UPROPERTY()
	bool bInit = false;
};

USTRUCT()
struct FRedCProjectileInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	TSubclassOf<ARedCProjectile> ProjectileClass = nullptr; // Projectile 클래스

	UPROPERTY()
	FVector StartLocation = FVector::ZeroVector; // Projectile 생성 지점 벡터

	UPROPERTY()
	FVector EndLocation = FVector::ZeroVector; // Projectile 목표 지점 벡터

	UPROPERTY()
	FVector Direction = FVector::ZeroVector; // Projectile 방향 벡터

	UPROPERTY()
	FRotator Rotation = FRotator::ZeroRotator; // Projectile 회전 벡터
	
	UPROPERTY()
	float Speed = 0.f; // Projectile 속도
	
	UPROPERTY()
	float Distance = 0.f; // Projectile 거리

	UPROPERTY()
	float Time = 0.f; // Projectile 시간

	UPROPERTY()
	FHitReactMessage HitReactMessage = FHitReactMessage();
};

UCLASS()
class PROJECTREDC_API UHitReactEventObject : public UObject
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetHitReactMessage(const FHitReactMessage& InHitReactMessage) { HitReactMessage = InHitReactMessage; }
	FORCEINLINE FHitReactMessage GetHitReactMessage() const { return HitReactMessage; }

private:
	UPROPERTY()
	FHitReactMessage HitReactMessage;
};

UCLASS()
class PROJECTREDC_API UProjectileSpawnEventObject : public UObject
{
	GENERATED_BODY()

public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	
	UPROPERTY()
	FRedCProjectileInfo ProjectileInfo;
};

USTRUCT(BlueprintType)
struct FCustomGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FVector CustomVector = FVector::ZeroVector;

	UPROPERTY()
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY()
	EArmorType ArmorType = EArmorType::None;
	
public:
	FCustomGameplayEffectContext() { }

	void SetCustomVector(const FVector& InVector) { CustomVector = InVector; }
	FVector GetCustomVector() const { return CustomVector; }

	void SetWeaponType(const EWeaponType InWeaponType) { WeaponType = InWeaponType; }
	EWeaponType GetWeaponType() const { return WeaponType; }

	void SetArmorType(const EArmorType InArmorType) { ArmorType = InArmorType; }
	EArmorType GetArmorType() const { return ArmorType; }

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override
	{
		FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

		Ar << CustomVector;
		Ar << WeaponType;

		bOutSuccess = true;

		return true;
	}

	virtual FCustomGameplayEffectContext* Duplicate() const override
	{
		FCustomGameplayEffectContext* NewContext = new FCustomGameplayEffectContext(*this);

		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
};

template<>
struct TStructOpsTypeTraits<FCustomGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FCustomGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
