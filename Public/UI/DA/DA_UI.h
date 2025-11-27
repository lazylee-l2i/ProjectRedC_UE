// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Type/HUDType.h"
#include "UI/Widget/_Base/Component/BaseIntro.h"
#include "DA_UI.generated.h"

class UBaseLoadingWidget;
class UBaseMenuWidget;
class UBaseInGameHUDWidget;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UDA_UI : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	// Loading
	UPROPERTY(EditDefaultsOnly, Category="UI|Intro")
	TSubclassOf<UBaseLoadingWidget> LoadingHUDClass;
	
	// Intro
	UPROPERTY(EditDefaultsOnly, Category="UI|Intro")
	TSubclassOf<UBaseIntro> IntroHUDClass;
	
	// 모드별 HUD 클래스
	UPROPERTY(EditDefaultsOnly, Category="UI|GameMode")
	TMap<EProjectUIType, TSubclassOf<UBaseInGameHUDWidget>> ModeHUDClasses;
	
	// Menu HUD 클래스
	UPROPERTY(EditDefaultsOnly, Category="Menu")
	TMap<EProjectMenuType, TSubclassOf<UBaseMenuWidget>> MenuHUDClasses;

	TSubclassOf<UBaseLoadingWidget> GetLoadingWidgetClass() const;
	TSubclassOf<UBaseIntro> GetIntroWidgetClass() const;
	TSubclassOf<UBaseInGameHUDWidget> GetInGameHUDClass(EProjectUIType Mode) const;
	TSubclassOf<UBaseMenuWidget> GetMenuHUDClass(EProjectMenuType Type) const;
	
	
};
