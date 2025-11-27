// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/_Base/Component/BaseBattleTextBlock.h"

#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"


void UBaseBattleTextBlock::NativePreConstruct()
{
	Super::NativePreConstruct();

	

		
	if (BaseText)
	{
		BaseText->SetText(InitialText);

		FSlateFontInfo FontInfo = BaseText->GetFont();
		FontInfo.Size = FontSize;
		BaseText->SetFont(FontInfo);
	}
}

void UBaseBattleTextBlock::NativeConstruct()
{
	Super::NativeConstruct();

	if (BaseText)
	{
		BaseText->SetText(InitialText);

		FSlateFontInfo FontInfo = BaseText->GetFont();
		FontInfo.Size = FontSize;
		BaseText->SetFont(FontInfo);
	}
}
