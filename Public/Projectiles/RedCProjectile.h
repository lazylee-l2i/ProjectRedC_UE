// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/RedCAbilityTypes.h"
#include "RedCProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

struct FProjectileParams;

UCLASS()
class PROJECTREDC_API ARedCProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	ARedCProjectile();

	void InitializeProjectile(const FRedCProjectileInfo& ProjectileInfo);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	bool CheckSameTeam(AActor* OtherActor);
	
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;
	
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> ProjectileCollision;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> WorldCollision;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Type")
	EWeaponType WeaponType;

	FHitReactMessage HitReactMessage;
};
