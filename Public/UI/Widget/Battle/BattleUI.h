// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/BattleShopWidget.h"
#include "Component/GamePlayCountDownWidget.h"
#include "Component/HPGaugeWidget.h"
#include "Component/HPWidget.h"
#include "Component/LevelWidget.h"
#include "Component/RespawnCountDownWidget.h"
#include "Component/SkillBoxWidget.h"
#include "Component/StaminaGaugeWidget.h"
#include "Component/TowerStatusWidget.h"
#include "UI/Widget/_Base/BaseInGameHUDWidget.h"
#include "UI/Widget/_Base/Component/BaseBattleResult.h"
#include "BattleUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBattleUI : public UBaseInGameHUDWidget
{
	GENERATED_BODY()

public:
	void ShowGameWinUI();
	void ShowGameLoseUI();
	void ShowGameDrawUI();
	void ShowRespawnCount();
	void HiddenRespawnCount();
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UHPGaugeWidget* HPGauge;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UStaminaGaugeWidget* StaminaGauge;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UBattleShopWidget* BattleShopWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	USkillBoxWidget* SkillBoxWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	ULevelWidget* LevelWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UHPWidget* HPWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UGamePlayCountDownWidget* GamePlayCountDownWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UBaseBattleResult* BattleResultWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTowerStatusWidget* MyTeamTowerStatus;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTowerStatusWidget* EnemyTeamTowerStatus;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	URespawnCountDownWidget* RespawnCountDownWidget;
};
