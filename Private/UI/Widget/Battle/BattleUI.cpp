// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/BattleUI.h"


void UBattleUI::ShowGameWinUI()
{
	BattleResultWidget->WidgetSwitcher_Result->SetActiveWidgetIndex(0);
	BattleResultWidget->WidgetSwitcher_Result->SetVisibility(ESlateVisibility::Visible);
	BattleResultWidget->PlayMoveDownAnim();
}

void UBattleUI::ShowGameLoseUI()
{
	BattleResultWidget->WidgetSwitcher_Result->SetActiveWidgetIndex(1);
	BattleResultWidget->WidgetSwitcher_Result->SetVisibility(ESlateVisibility::Visible);
	BattleResultWidget->PlayMoveDownAnim();
}

void UBattleUI::ShowGameDrawUI()
{
	BattleResultWidget->WidgetSwitcher_Result->SetActiveWidgetIndex(2);
	BattleResultWidget->WidgetSwitcher_Result->SetVisibility(ESlateVisibility::Visible);
	BattleResultWidget->PlayMoveDownAnim();
}

void UBattleUI::ShowRespawnCount()
{
	RespawnCountDownWidget->Overlay_RespawnCount->SetVisibility(ESlateVisibility::Visible);
}

void UBattleUI::HiddenRespawnCount()
{
	RespawnCountDownWidget->Overlay_RespawnCount->SetVisibility(ESlateVisibility::Hidden);
}
