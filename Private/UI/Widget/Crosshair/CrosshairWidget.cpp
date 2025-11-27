// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Crosshair/CrosshairWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"


void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsCrosshairYAtLimit = false;

	if (Crosshair)
	{
		// Widget 생성 시 Crosshair의 초기 위치 값 저장
		if (const UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Crosshair->Slot))
		{
			CrosshairPosition = CanvasPanelSlot->GetPosition();
		}
		
		// Widget 생성 시 Crosshair의 회전 방향 설정
		RotateDirection = ERotateDirection::Clockwise;
	}
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Crosshair 각도 갱신
	UpdateCrosshairOuterAngle(InDeltaTime);
}

void UCrosshairWidget::SetCrosshairPosition(const FVector2D& InPosition)
{
	if (!Crosshair)
	{
		return;
	}

	// CanvasPanel 좌표계 기준 위치 변경
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Crosshair->Slot))
	{
		const FVector2D CanvasSize = GetCachedGeometry().GetLocalSize();

		// Y축 이동 범위 설정
		const float MinOffsetY = -(CanvasSize.Y * 0.5f) + CanvasSize.Y * (1.0f - MinY);
		const float MaxOffsetY = +(CanvasSize.Y * 0.5f) - CanvasSize.Y * (1.0f - MaxY);

		// 이동 범위 내 값으로 Clamp
		CrosshairPosition.Y = FMath::Clamp(InPosition.Y, MinOffsetY, MaxOffsetY);

		// Crosshair 위치 변경
		CanvasPanelSlot->SetPosition(CrosshairPosition);

		// 범위 제한에 걸릴 경우 플래그 갱신
		if (CrosshairPosition.Y == MinOffsetY || CrosshairPosition.Y == MaxOffsetY)
		{
			bIsCrosshairYAtLimit = true;
		}
		else
		{
			bIsCrosshairYAtLimit = false;
		}
	}
}

void UCrosshairWidget::SetCrosshairRotateDirection(float Value)
{
	if (!Crosshair)
	{
		return;
	}

	if (Value > 0.f)
	{
		// Crosshair의 회전 방향을 시계 방향으로 설정
		RotateDirection = ERotateDirection::Clockwise;
	}
	else if (Value < 0.f)
	{
		// Crosshair의 회전 방향을 반시계 방향으로 설정
		RotateDirection = ERotateDirection::Counterclockwise;
	}
}

void UCrosshairWidget::UpdateCrosshairOuterAngle(float DeltaTime)
{
	if (CrosshairOuter)
	{
		// Crosshair 현재 각도
		float Angle = CrosshairOuter->GetRenderTransformAngle();

		switch (RotateDirection)
		{
		case ERotateDirection::Clockwise:
			Angle += DeltaTime * RotateSpeed;
			break;
		case ERotateDirection::Counterclockwise:
			Angle -= DeltaTime * RotateSpeed;
			break;
		}

		CrosshairOuter->SetRenderTransformAngle(Angle);
	}
}

FVector2D UCrosshairWidget::GetCrosshairPosition() const
{
	return CrosshairPosition;
}

FVector2D UCrosshairWidget::GetCrosshairScreenPosition() const
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;

	if (const APlayerController* PC = GetOwningPlayer())
	{
		// Viewport Size 가져오기
		int32 SizeX ,SizeY;
		PC->GetViewportSize(SizeX, SizeY);
		const FVector2D ViewportSize = FVector2D(SizeX, SizeY);

		// DPI Scale 가져오기
		const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);

		// Viewport 중앙 좌표 계산
		const FVector2D ViewportCenter = ViewportSize * 0.5f;

		// 현재 CrosshairPosition Offset에 DPI Scale 적용
		const FVector2D ScaledOffset = CrosshairPosition * DPIScale;

		// 스크린 좌표 계산
		ScreenPosition = ViewportCenter + ScaledOffset;
	}

	return ScreenPosition;
}
