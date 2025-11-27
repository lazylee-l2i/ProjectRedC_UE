// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Battle/OverheadWidget.h"
#include "OverheadInfoWidget.generated.h"

class UVerticalBox;
class UTextBlock;

struct FGenericTeamId;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UOverheadInfoWidget : public UOverheadWidget
{
	GENERATED_BODY()
	
public:
	void SetLevelText(int32 Level);
	void SetCharacterNameText(FName CharacterName);
	void SetPlayerNameText(FName PlayerName);
	
	virtual void SetTeamColor(bool bIsSameTeam) override;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> CharacterNameText;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;

	// 0 : Enemy, 1 : Friend
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Text Color")
	TArray<FSlateColor> TeamColor;
};
