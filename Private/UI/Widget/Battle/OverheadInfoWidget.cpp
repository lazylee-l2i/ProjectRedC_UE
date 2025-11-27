// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/OverheadInfoWidget.h"

#include "Components/TextBlock.h"


void UOverheadInfoWidget::SetLevelText(int32 Level)
{
	const FText FormattedText = FText::Format(FText::FromString(TEXT("[Lv. {0}")), FText::AsNumber(Level));
	
	LevelText->SetText(FormattedText);
}

void UOverheadInfoWidget::SetCharacterNameText(FName CharacterName)
{
	const FText FormattedText = FText::Format(FText::FromString(TEXT("{0}]")), FText::FromName(CharacterName));
	
	CharacterNameText->SetText(FormattedText);
}

void UOverheadInfoWidget::SetPlayerNameText(FName PlayerName)
{
	const FText FormattedText = FText::Format(FText::FromString(TEXT("{0}")), FText::FromName(PlayerName));
	
	PlayerNameText->SetText(FormattedText);
}

void UOverheadInfoWidget::SetTeamColor(bool bIsSameTeam)
{
	LevelText->SetColorAndOpacity(TeamColor[bIsSameTeam]);
	CharacterNameText->SetColorAndOpacity(TeamColor[bIsSameTeam]);
	PlayerNameText->SetColorAndOpacity(TeamColor[bIsSameTeam]);
}
