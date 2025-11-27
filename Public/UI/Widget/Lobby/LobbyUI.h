// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/MatchingWidget.h"
#include "UI/Widget/_Base/BaseInGameHUDWidget.h"
#include "UI/Widget/_Base/Component/BaseCountDown.h"
#include "LobbyUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ULobbyUI : public UBaseInGameHUDWidget
{
	GENERATED_BODY()

public:
	//virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	//virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
public:
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UMatchingWidget* MatchingWidget;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UBaseCountDown* MatchCountDownWidget;
};
