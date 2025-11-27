// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Manager/UIManagerSubsystem.h"

#include "Animation/WidgetAnimation.h"
#include "Components/WidgetSwitcher.h"
#include "Frameworks/RedCPlayerStateBase.h"
#include "Frameworks/GameMode/GameModeArena.h"
#include "Frameworks/GameMode/GameModeLobby.h"
#include "Frameworks/PlayerState/PlayerStateLobby.h"
#include "Types/RedCLogChannels.h"
#include "UI/DA/DA_UI.h"
#include "UI/Widget/Battle/BattleUI.h"
#include "UI/Widget/Settings/MainMenuUI.h"
#include "UI/Widget/_Base/BaseInGameHUDWidget.h"
#include "UI/Widget/_Base/BaseLoadingWidget.h"
#include "UI/Widget/_Base/BaseMenuWidget.h"



void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!UISettings)
	{
		// 폴백 경로 지정 (콘텐츠 브라우저 Copy Reference)
		static const TCHAR* Path = TEXT("/Game/Blueprint/UI/DA/DA_UI.DA_UI");
		UISettings = Cast<UDA_UI>(StaticLoadObject(UDA_UI::StaticClass(), nullptr, Path));
	}
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMap);
}


void UUIManagerSubsystem::OnPostLoadMap(UWorld* World)
{
	if (CachedLoadingWidget && !CachedLoadingWidget->IsInViewport())
	{
		CachedLoadingWidget->AddToViewport(999);
	}
}

void UUIManagerSubsystem::CreateMenuHUD(ARedCPlayerController* PC, EProjectMenuType MenuType)
{
	if (!UISettings)
	{
		//UE_LOG(LogRedCWidget, Error, TEXT("[UIManager] UISettings is null (CreateMenuHUD)"));
		return;
	}

	// DA에서 메뉴 위젯 클래스 가져오기 (반환형은 프로젝트에 맞춰 조정: UUserWidget / UBaseMenuWidget 등)
	TSubclassOf<UUserWidget> MenuClass = UISettings->GetMenuHUDClass(MenuType);
	if (!MenuClass)
	{
		//UE_LOG(LogRedCWidget, Error, TEXT("[UIManager] No Menu class for MenuType=%d"), (int32)MenuType);
		return;
	}

	// 기존 메뉴 제거(중복 방지)
	RemoveCurrentMenuHUD(PC);

	
	// 생성
	if (UBaseMenuWidget* NewMenu = CreateWidget<UBaseMenuWidget>(PC, MenuClass))
	{
		NewMenu->AddToViewport();
		CurrentMenuHUD = NewMenu;
        
		PC->SetInputModeEx(EMGInputMode::UIOnly);
	}
}

int32 UUIManagerSubsystem::ShowIntroAnim() const
{
	if (!UISettings) return 0;

	if (TSubclassOf<UBaseIntro> IntroClass = UISettings->GetIntroWidgetClass())
	{
		UBaseIntro* IntroWidget = CreateWidget<UBaseIntro>(GetWorld(), IntroClass);

		IntroWidget->AddToViewport();
		IntroWidget->PlayAnimation(IntroWidget->FadeInAndOutAnim, 0, 1, EUMGSequencePlayMode::Forward, 1);
		return IntroWidget->FadeInAndOutAnim->GetEndTime();
	}
	return 0;
}

void UUIManagerSubsystem::ShowMainMenuFadeIn()
{
	if (TObjectPtr<UMainMenuUI> MainMenu = Cast<UMainMenuUI>(CurrentMenuHUD))
	{
		MainMenu->PlayAnimation(MainMenu->EnterMainMenuFadeInAnim, 0, 1, EUMGSequencePlayMode::Forward, 1);
	}
}


void UUIManagerSubsystem::RemoveCurrentMenuHUD(ARedCPlayerController* PC)
{
	if (CurrentMenuHUD)
	{
		CurrentMenuHUD->RemoveFromParent();
		CurrentMenuHUD = nullptr;

		PC->SetInputModeEx(EMGInputMode::GameOnly);
	}
}

void UUIManagerSubsystem::CreateInGameHUDForMode(ARedCPlayerController* PC, EProjectUIType Mode)
{
	if (!UISettings)
	{
		return;
	}

	TSubclassOf<UBaseInGameHUDWidget> HUDClass = UISettings->GetInGameHUDClass(Mode);
	if (!HUDClass)
	{
		return;
	}

	ApplyUIClass(PC, HUDClass);
}

void UUIManagerSubsystem::RemoveCurrentInGameHUD(ARedCPlayerController* PC)
{
	if (CurrentHUD)
	{
		// 뷰포트 제거
		CurrentHUD->RemoveFromParent();
		CurrentHUD = nullptr;

		PC->SetInputModeEx(EMGInputMode::GameOnly);
	}
}

void UUIManagerSubsystem::ApplyUIClass(ARedCPlayerController* PC, TSubclassOf<UBaseInGameHUDWidget> HUDClass)
{
	if (!HUDClass) return;

	// 동일 클래스면 스킵
	if (CurrentHUD && CurrentHUD->GetClass() == HUDClass) return;

	ARedCPlayerStateBase* PS = PC->GetPlayerState<ARedCPlayerStateBase>();
	if (!PS)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(RetryTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(
				RetryTimerHandle,
				FTimerDelegate::CreateUObject(this, &UUIManagerSubsystem::ApplyUIClass, PC, HUDClass),
				0.1f,
				true
			);
		}
		return;
	}

	// PS 준비완료 됐으면 타이머 정리
	if (GetWorld()->GetTimerManager().IsTimerActive(RetryTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(RetryTimerHandle);
	}

	
	// 기존 CurrentInGameHUD 제거
	RemoveCurrentInGameHUD(PC);
	
	// 생성
	if (UBaseInGameHUDWidget* NewHUD = CreateWidget<UBaseInGameHUDWidget>(PC, HUDClass))
	{
		NewHUD->AddToViewport();
		CurrentHUD = NewHUD;
		
		CurrentHUD->PlayAnimation(CurrentHUD->EnterFadeInAnim, 0, 1, EUMGSequencePlayMode::Forward, 1);
		RemoveLoadingWidget();
	}
}

void UUIManagerSubsystem::ShowLoadingWidget(UGameInstance* GI)
{
	if (TSubclassOf<UBaseLoadingWidget> LoadingClass = UISettings->GetLoadingWidgetClass())
	{
		CachedLoadingWidget = CreateWidget<UBaseLoadingWidget>(GI, LoadingClass);
		CachedLoadingWidget->AddToViewport(999);
	}
}

void UUIManagerSubsystem::RemoveLoadingWidget()
{
	if (!CachedLoadingWidget) return;
	CachedLoadingWidget->RemoveFromParent();
	CachedLoadingWidget = nullptr;
}

void UUIManagerSubsystem::BattleShopPageChange()
{
	if (UBattleUI* BattleUI = Cast<UBattleUI>(CurrentHUD))
	{
		if (UBattleShopWidget* ShopWidget = BattleUI->BattleShopWidget)
		{
			ShopWidget->BattleShopPageChange();
		}
	}
}

int32 UUIManagerSubsystem::GetCurrentBattleShopPageIndex()
{
	if (UBattleUI* BattleUI = Cast<UBattleUI>(CurrentHUD))
	{
		if (UBattleShopWidget* ShopWidget = BattleUI->BattleShopWidget)
		{
			return ShopWidget->Switcher_Pages->GetActiveWidgetIndex();
		}
	}
	return -1;
}

ARedCPlayerController* UUIManagerSubsystem::GetLocalRedCPlayerController() const
{
	if (const ULocalPlayer* LP = GetLocalPlayer())
	{
		if (APlayerController* PC = LP->GetPlayerController(LP->GetWorld()))
		{
			return Cast<ARedCPlayerController>(PC);
		}
	}
	return nullptr;
}

