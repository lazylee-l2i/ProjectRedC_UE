// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrosshairComponent.generated.h"

USTRUCT()
struct FLocationAndDirection
{
	GENERATED_BODY()

	UPROPERTY()
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY()
	FVector WorldDirection = FVector::ZeroVector;
};

class UCrosshairWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREDC_API UCrosshairComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCrosshairComponent();
	
	void CreateCrosshairWidget();
	void RemoveCrosshairWidget();

	void AddPitchInput(float Value);
	void AddYawInput(float Value);
	
private:
	FLocationAndDirection DeprojectScreenPositionToWorld() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Crosshair")
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UCrosshairWidget> CrosshairWidget;

public:
	UFUNCTION(BlueprintPure)
	FVector GetCrosshairDirection(const FVector& StartLocation, float Distance) const;

	UFUNCTION(BlueprintPure)
	bool IsCrosshairYAtLimit() const;
};
