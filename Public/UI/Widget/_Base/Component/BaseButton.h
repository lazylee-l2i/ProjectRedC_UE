// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonSelected, int, Index);
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBaseButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonClicked OnButtonClicked;
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonSelected OnButtonSelected;

public:
	// 기본 버튼설정
	// 버튼이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button")
	FText ButtonName = FText::FromString(TEXT(""));
	// 폰트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button")
	FSlateFontInfo ButtonFont;
	// 배경색
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button")
	FLinearColor ButtonBackgroundColor = FLinearColor::White;
	// 클릭사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button")
	USoundBase* ClickSound = nullptr;

	// 호버 사용 여부 스위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|Hover")
	bool bUseHoverStyle = false;
	// 스위치가 켜졌을 때만 노출
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|Hover", meta=(EditCondition="bUseHoverStyle"))
	FLinearColor ButtonBackgroundColor_Hover = FLinearColor(FColor::FromHex(TEXT("#7B7B7B")));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|Hover", meta=(EditCondition="bUseHoverStyle"))
	FLinearColor ButtonBackgroundColor_Unhover = FLinearColor::Black;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|Hover", meta=(EditCondition="bUseHoverStyle"))
	USoundBase* HoverSound = nullptr;

};
