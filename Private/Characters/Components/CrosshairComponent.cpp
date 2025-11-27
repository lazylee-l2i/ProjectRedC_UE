// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/CrosshairComponent.h"

#include "Types/RedCLogChannels.h"
#include "UI/Widget/Crosshair/CrosshairWidget.h"


UCrosshairComponent::UCrosshairComponent()
	: CrosshairWidget(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCrosshairComponent::CreateCrosshairWidget()
{
	check(CrosshairWidgetClass);

	const APawn* Pawn = GetOwner<APawn>();
	APlayerController* PC = Pawn ? Pawn->GetController<APlayerController>() : nullptr;
	if (!PC || PC->IsNetMode(NM_DedicatedServer))
	{
		return;
	}

	if (!CrosshairWidget.Get())
	{
		CrosshairWidget = CreateWidget<UCrosshairWidget>(PC, CrosshairWidgetClass);
	}

	CrosshairWidget->AddToViewport();
}

void UCrosshairComponent::RemoveCrosshairWidget()
{
	if (CrosshairWidget.Get())
	{
		CrosshairWidget->RemoveFromParent();
	}
}

void UCrosshairComponent::AddPitchInput(float Value)
{
	if (CrosshairWidget.Get())
	{
		// 입력 값을 현재 Crosshair의 Y축 위치 값에 반영
		const FVector2D CurrentPosition = CrosshairWidget->GetCrosshairPosition();
		const FVector2D NewPosition = FVector2D(CurrentPosition.X, CurrentPosition.Y + Value);

		// Crosshair의 위치를 변경
		CrosshairWidget->SetCrosshairPosition(NewPosition);
	}
}

void UCrosshairComponent::AddYawInput(float Value)
{
	if (CrosshairWidget.Get())
	{
		// Crosshair의 회전 방향을 변경 
		CrosshairWidget->SetCrosshairRotateDirection(Value);
	}
}

FLocationAndDirection UCrosshairComponent::DeprojectScreenPositionToWorld() const
{
	FLocationAndDirection LocationAndDirection;

	const APawn* Owner = GetOwner<APawn>();
	const APlayerController* PC = Owner ? Owner->GetController<APlayerController>() : nullptr;

	if (PC && CrosshairWidget.Get())
	{
		const FVector2D CrosshairScreenPosition = CrosshairWidget->GetCrosshairScreenPosition();

		// Crosshair의 스크린 좌표를 기준으로 카메라의 위치 벡터와 방향 벡터 값 가져오기
		PC->DeprojectScreenPositionToWorld(
			CrosshairScreenPosition.X, CrosshairScreenPosition.Y,
			LocationAndDirection.WorldLocation, LocationAndDirection.WorldDirection);
	}
	else
	{
		UE_LOG(LogRedC, Warning, TEXT(
			"Player Controller or Crosshair Widget is null, Location and Direction return zero vectors"));
	}

	return LocationAndDirection;
}

FVector UCrosshairComponent::GetCrosshairDirection(const FVector& StartLocation, float Distance) const
{
	const APawn* Owner = GetOwner<APawn>();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}
	
	const FLocationAndDirection LocationAndDirection = DeprojectScreenPositionToWorld();
	const FVector Offset = StartLocation - LocationAndDirection.WorldLocation;
	const FVector EndLocation = LocationAndDirection.WorldLocation + LocationAndDirection.WorldDirection * (Distance + Offset.Length());

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	// Camera 기준 Line Trace
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, LocationAndDirection.WorldLocation, EndLocation, ECC_Visibility, QueryParams);

	// Trace Hit 결과에 따라 Target Location 설정
	FVector TargetLocation = bHit ? HitResult.ImpactPoint : EndLocation;

	// Trace Hit 결과가 캐릭터 보다 뒤쪽에 있을 경우 보정 (캐릭터와 카메라 사이에서 Hit)
	if (FVector::DotProduct(TargetLocation - StartLocation, LocationAndDirection.WorldDirection) < 0)
	{
		TargetLocation = EndLocation;
	}

	// 방향 벡터 반환
	return (TargetLocation - StartLocation).GetSafeNormal();
}

bool UCrosshairComponent::IsCrosshairYAtLimit() const
{
	if (CrosshairWidget.Get())
	{
		return CrosshairWidget->GetIsCrosshairYAtLimit();
	}

	return false;
}
