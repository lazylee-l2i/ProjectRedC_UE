// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/RedCPlayerController.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/DA/DA_UI.h"
#include "UI/Type/HUDType.h"
#include "UIManagerSubsystem.generated.h"

class UPlayerMatchingViewModel;
class UBaseInGameHUDWidget;
class UBaseMenuWidget;
class UDA_UI;
/**
 * 
 */
UCLASS(BlueprintTYpe, Blueprintable)
class PROJECTREDC_API UUIManagerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnPostLoadMap(UWorld* World);

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TObjectPtr<UDA_UI> UISettings = nullptr;

private:
	UPROPERTY()
	UBaseLoadingWidget* CachedLoadingWidget = nullptr;

public:
	/** Menu용 */
	//MenuHUD 생성
	UFUNCTION(BlueprintCallable)
	void CreateMenuHUD(ARedCPlayerController* PC, EProjectMenuType MenuType);
	int32 ShowIntroAnim() const;

	// MenuHUD 제거
	UFUNCTION(BlueprintCallable)
	void RemoveCurrentMenuHUD(ARedCPlayerController* PC);

	/** InGame용 */
	// InGameHUD 생성
	UFUNCTION(BlueprintCallable)
	void CreateInGameHUDForMode(ARedCPlayerController* PC, EProjectUIType Mode);

	// InGameHUD 제거
	UFUNCTION(BlueprintCallable, Category="UI")
	void RemoveCurrentInGameHUD(ARedCPlayerController* PC);

	// InGameHUD 교체
	UFUNCTION(BlueprintCallable, Category="UI")
	void ApplyUIClass(ARedCPlayerController* PC, TSubclassOf<UBaseInGameHUDWidget> HUDClass);

	// Loading
	void ShowLoadingWidget(UGameInstance* GI);
	void RemoveLoadingWidget();

public:
	// 현재 표시 중인 MenuHUD
	UPROPERTY(Transient)
	TObjectPtr<UBaseMenuWidget> CurrentMenuHUD = nullptr;
	
	// 현재 표시 중인 InGameHUD
	UPROPERTY(Transient)
	TObjectPtr<UBaseInGameHUDWidget> CurrentHUD = nullptr;
	
	// 간단 재시도용 타이머
	FTimerHandle RetryTimerHandle;
	bool bRetryScheduled = false;

	// 임시 서버 트래블 주소 / 서버트래블 해도 전역으로 가지고있는게 UI매니저라 임시저장
	UPROPERTY()
	FString TempServerIP = "";

	void BattleShopPageChange();
	int32 GetCurrentBattleShopPageIndex();

public:
	/** Getter **/
	TSubclassOf<UBaseIntro> GetCurrentIntroWidget() const {return UISettings->IntroHUDClass;};
	TObjectPtr<UBaseMenuWidget> GetCurrentMenuHUD(){return CurrentMenuHUD;};
	TObjectPtr<UBaseInGameHUDWidget> GetCurrentHUD(){return CurrentHUD;};

	ARedCPlayerController* GetLocalRedCPlayerController() const;

	// FadeIn
	void ShowMainMenuFadeIn();
};
