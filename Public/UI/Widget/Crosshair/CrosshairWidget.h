// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

UENUM(BlueprintType)
enum class ERotateDirection : uint8
{
	Clockwise,
	Counterclockwise
};

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetCrosshairPosition(const FVector2D& InPosition);

	FVector2D GetCrosshairPosition() const;
	FVector2D GetCrosshairScreenPosition() const;

	FORCEINLINE bool GetIsCrosshairYAtLimit() const { return bIsCrosshairYAtLimit; }

private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true, BindWidget = "Crosshair"))
	TObjectPtr<class UOverlay> Crosshair;
	
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Crosshair Range", Meta = (ClampMin = 0.5, ClampMax = 1.0))
	float MinY;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Crosshair Range", Meta = (ClampMin = 0.5, ClampMax = 1.0))
	float MaxY;

	bool bIsCrosshairYAtLimit;

	FVector2D CrosshairPosition;
	
public:
	void SetCrosshairRotateDirection(float Value);

private:
	void UpdateCrosshairOuterAngle(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true, BindWidget = "CrosshairOuter"))
	TObjectPtr<class UImage> CrosshairOuter;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Crosshair Rotate", Meta = (ClampMin = 0))
	float RotateSpeed;

	ERotateDirection RotateDirection;
};
