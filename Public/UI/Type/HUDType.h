#pragma once

#include "CoreMinimal.h"
#include "HUDType.generated.h"
UENUM()
enum class EProjectGameMode : uint8
{
	Default,
	MainMenuMode,
	LobbyMode,
	InGameMode
};

UENUM(BlueprintType)
enum class EProjectUIType : uint8
{
	LobbyUI,
	CharacterSelectionUI,
	BattleUI
};

UENUM(BlueprintType)
enum class EProjectMenuType : uint8
{
	MainMenu,
	PauseMenu,
	SettingMenu,
	CreditMenu
};


