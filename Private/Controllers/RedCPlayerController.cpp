// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/RedCPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/Components/CrosshairComponent.h"
#include "Components/WidgetSwitcher.h"
#include "Frameworks/GameMode/GameModeArena.h"
#include "Frameworks/RedCGameStateBase.h"
#include "Frameworks/GameMode/GameModeLobby.h"
#include "Frameworks/GameState/GameStateLobby.h"
#include "Frameworks/PlayerState/PlayerStateLobby.h"
#include "Input/InputConfig.h"
#include "Net/UnrealNetwork.h"
#include "UI/Manager/UIManagerSubsystem.h"
#include "UI/Type/HUDType.h"
#include "UI/Widget/Battle/BattleUI.h"
#include "UI/Widget/Lobby/LobbyUI.h"

ARedCPlayerController::ARedCPlayerController()
	: UIManager(nullptr), CurrentInputMode(EMGInputMode::None)
{
	PitchSensitivity = 0.1f;
	YawSensitivity = 0.1f;
}

void ARedCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		UIManager = LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
		CreateHudWidget();
	}
}

void ARedCPlayerController::ClearInputMappingContext()
{
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
}

void ARedCPlayerController::SetupInputMappingContext()
{
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	for (const FGameplayTag& ContextTag : ContextTags)
	{
		const TPair<UInputMappingContext*, int32> InputMapping = InputConfig->FindInputMappingContextForTag(ContextTag);
		if (InputMapping.Key /* InputMappingContext */)
		{
			Subsystem->AddMappingContext(InputMapping.Key /* InputMappingContext */, InputMapping.Value /* Priority */);
		}
	}
}

void ARedCPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// PS 기준으로 바인딩 키 세팅
	if (GetPlayerState<APlayerStateLobby>())
	{
		InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &ThisClass::RegisterPlayerController);
		InputComponent->BindKey(EKeys::LeftAlt, IE_Pressed, this, &ThisClass::OnAltPressedToggle);
	}
	if (GetPlayerState<APlayerStateArena>())
	{
		InputComponent->BindKey(EKeys::F2, IE_Pressed, this, &ThisClass::Server_EndGame);
	}
}

void ARedCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ARedCPlayerController, bHasBeenRegistered, COND_None, REPNOTIFY_Always);
}

#pragma region Server

void ARedCPlayerController::RegisterPlayerController()
{
	if (bHasBeenRegistered)
	{
		return;
	}
	
	if (AGameStateLobby* GS = Cast<AGameStateLobby>(GetWorld()->GetGameState()))
	{
		// 매칭완료시 다른매칭 취소 처리
		if (!GS->MatchStarted)
		{
			Server_RegisterPlayer();

			if (UIManager)
			{
				ULobbyUI* LobbyUI = Cast<ULobbyUI>(UIManager->GetCurrentHUD());
				LobbyUI->MatchingWidget->WidgetSwitcher_Matching->SetActiveWidgetIndex(1);

				bHasBeenRegistered = true;
			}
		}
		else
		{
			if (UIManager)
			{
				ULobbyUI* LobbyUI = Cast<ULobbyUI>(UIManager->GetCurrentHUD());
				LobbyUI->MatchingWidget->WidgetSwitcher_Matching->SetActiveWidgetIndex(2);
			}
			
			FTimerHandle F;
			GetWorld()->GetTimerManager().SetTimer(F, this, &ThisClass::SetInitMatchingWidget, 2.f, false);
		}
	}
}


void ARedCPlayerController::Server_EndGame_Implementation()
{
	if (AGameModeArena* GM = Cast<AGameModeArena>(GetWorld()->GetAuthGameMode()))
	{
		GM->HandleEndGame(ETeamType::None);
	}
}

void ARedCPlayerController::Server_RegisterPlayer_Implementation()
{
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if (AGameModeLobby* GM = Cast<AGameModeLobby>(GetWorld()->GetAuthGameMode()))
		{
			bHasBeenRegistered = GM->RegisterPlayerController(this);
		}
	}
}

#pragma endregion

#pragma region Widget

void ARedCPlayerController::CreateHudWidget()
{
	if (UIManager)
	{
		if (const ARedCGameStateBase* GS = GetWorld()->GetGameState<ARedCGameStateBase>())
		{
			switch (GS->ProjectGameMode)
			{
			case EProjectGameMode::Default :
				break;
			case EProjectGameMode::MainMenuMode :
				{
					int32 PlayTime = UIManager->ShowIntroAnim();
					FTimerHandle TempHandle;
					GetWorld()->GetTimerManager().SetTimer(
						TempHandle,
						[this]() 
						{
							UIManager->CreateMenuHUD(this, EProjectMenuType::MainMenu);
							UIManager->ShowMainMenuFadeIn();
						},
						PlayTime, /* Anim 길이만큼 있다가 실행*/
						false    
					);
					break;
				}
			case EProjectGameMode::LobbyMode :
				UIManager->CreateInGameHUDForMode(this,EProjectUIType::LobbyUI);
				break;
			case EProjectGameMode::InGameMode :
				UIManager->CreateInGameHUDForMode(this,EProjectUIType::BattleUI);
				break;
			default:
				break;
			}
		}
	}
}

void ARedCPlayerController::Server_PlayerPickCharacter_Implementation(int CharacterID)
{
	// 캐릭터 선택창 위젯에서 가져온 정보
	if (APlayerStateLobby* PS = GetPlayerState<APlayerStateLobby>())
	{
		PS->SetPlayerTravelInfo_CharacterID(CharacterID);
	};
}

void ARedCPlayerController::Client_ShowGameWinnerUI_Implementation()
{
	if (UIManager)
	{
		UBattleUI* BattleUI = Cast<UBattleUI>(UIManager->GetCurrentHUD());
		BattleUI->ShowGameWinUI();
	}
}

void ARedCPlayerController::Client_ShowGameLoserUI_Implementation()
{
	if (UIManager)
	{
		UBattleUI* BattleUI = Cast<UBattleUI>(UIManager->GetCurrentHUD());
		BattleUI->ShowGameLoseUI();
	}
}

void ARedCPlayerController::Client_ShowGameDrawUI_Implementation()
{
	if (UIManager)
	{
		UBattleUI* BattleUI = Cast<UBattleUI>(UIManager->GetCurrentHUD());
		BattleUI->ShowGameDrawUI();
	}
}

void ARedCPlayerController::SetInitMatchingWidget()
{
	if (UIManager)
	{
		ULobbyUI* LobbyUI = Cast<ULobbyUI>(UIManager->GetCurrentHUD());
		LobbyUI->MatchingWidget->WidgetSwitcher_Matching->SetActiveWidgetIndex(0);
	}
}

void ARedCPlayerController::ShowRespawncount()
{
	if (UIManager)
	{
		UBattleUI* BattleUI = Cast<UBattleUI>(UIManager->GetCurrentHUD());
		BattleUI->ShowRespawnCount();
	}
}

void ARedCPlayerController::HiddenRespawncount()
{
	if (UIManager)
	{
		UBattleUI* BattleUI = Cast<UBattleUI>(UIManager->GetCurrentHUD());
		BattleUI->HiddenRespawnCount();
	}
}

void ARedCPlayerController::RemoveLoadingWidget()
{
	if (UIManager)
	{
		UIManager->RemoveLoadingWidget();
	}
}

void ARedCPlayerController::Client_StartLoadingWidget_Implementation()
{
	if (UIManager)
	{
		UGameInstance* GI = GetGameInstance();
		UIManager->ShowLoadingWidget(GI);
	}
}


void ARedCPlayerController::Client_ShowCharacterSelectionUI_Implementation()
{
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			UI->CreateInGameHUDForMode(this, EProjectUIType::CharacterSelectionUI);
			SetInputModeEx(EMGInputMode::UIOnly);
		}
	}
}
#pragma endregion

#pragma region WidgetFocus

void ARedCPlayerController::OnAltPressedToggle()
{
	bAltToggle = !bAltToggle;
	SetInputModeEx(bAltToggle ? EMGInputMode::GameAndUI : EMGInputMode::GameOnly);
	bShowMouseCursor = bAltToggle;
}

void ARedCPlayerController::ChangeGameModeGameAndUI()
{
	if (CurrentInputMode!=EMGInputMode::GameAndUI)
	{
		SetInputModeEx(EMGInputMode::GameAndUI);
	}
}

void ARedCPlayerController::ChangeGameModeGameOnly()
{
	if (CurrentInputMode!=EMGInputMode::GameOnly)
	{
		SetInputModeEx(EMGInputMode::GameOnly);
	}
}

void ARedCPlayerController::SetInputModeEx(EMGInputMode Mode)
{
	switch (Mode)
	{
	case EMGInputMode::GameOnly:
		{
			FInputModeGameOnly M;
			SetInputMode(M);
			SetShowMouseCursor(false);
			
			SetIgnoreMoveInput(false);
			SetIgnoreLookInput(false);

			CurrentInputMode = EMGInputMode::GameOnly;
			break;
		}
	case EMGInputMode::UIOnly:
		{
			FInputModeUIOnly M;
			//M.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			SetInputMode(M);
			SetShowMouseCursor(true);

			if (not UIManager->GetCurrentHUD())
			{
				return;
			}
			UIManager->GetCurrentHUD()->SetFocus();

			SetIgnoreMoveInput(true);
			SetIgnoreLookInput(true);
			
			CurrentInputMode = EMGInputMode::UIOnly;
			break;
		}
	case EMGInputMode::GameAndUI:
		{
			FInputModeGameAndUI M;
			M.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			SetInputMode(M);
			SetShowMouseCursor(true);

			if (not UIManager->GetCurrentHUD())
			{
				return;
			}
			UIManager->GetCurrentHUD()->SetFocus();
			
			SetIgnoreMoveInput(true);
			SetIgnoreLookInput(true);
			
			CurrentInputMode = EMGInputMode::GameAndUI;
			break;
		}
	default: break;
	}
}

#pragma endregion

#pragma region SetYawPitchInput
void ARedCPlayerController::AddPitchInput(float Value)
{
	float NewValue = Value * PitchSensitivity;
	
	if (const APawn* P = GetPawn())
	{
		// Look Input Action을 통해 Pitch 값을 Crosshair Component에 전달
		if (UCrosshairComponent* CrosshairComponent = P->FindComponentByClass<UCrosshairComponent>())
		{
			CrosshairComponent->AddPitchInput(NewValue * 10);

			// Crosshair가 이동 제한 범위에 도달한 경우 Pitch Input의 값을 0으로 초기화
			NewValue = CrosshairComponent->IsCrosshairYAtLimit() ? 0.f : NewValue * 0.25f;
		}
	}

	Super::AddPitchInput(NewValue);
}

void ARedCPlayerController::AddYawInput(float Value)
{
	const float NewValue = Value * YawSensitivity;

	if (const APawn* P = GetPawn())
	{
		// Look Input Action을 통해 Yaw 값을 Crosshair Component에 전달
		if (UCrosshairComponent* CrosshairComponent = P->FindComponentByClass<UCrosshairComponent>())
		{
			CrosshairComponent->AddYawInput(NewValue * 10);
		}
	}
	
	Super::AddYawInput(NewValue);
}
#pragma endregion