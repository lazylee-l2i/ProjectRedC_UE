// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/Component/OneKeySkillBox.h"

#include "Components/Image.h"


void UOneKeySkillBox::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Img_Icon)
	{
		Img_Icon->SetBrush(IconBrush);
		Img_Icon->SetDesiredSizeOverride(IconSize);
		Img_Icon->SetRenderTransform(IconRenderTransform);
	}
	if (Text_SkillName)
	{
		FSlateFontInfo FontInfo = Text_SkillName->GetFont();
		FontInfo.Size = Text_SkillFont;
		Text_SkillName->SetFont(FontInfo);
		Text_SkillName->SetJustification(ETextJustify::Left);
	}
	if (SkillCoolDownText)
	{
		SkillCoolDownText->SetVisibility(ESlateVisibility::Hidden);
	}
	if (!WhiteBG_Visible)
	{
		Img_WhiteBG->SetVisibility(ESlateVisibility::Hidden);
	}
	if (!BlackBG_Visible)
	{
		Img_BlackBG->SetVisibility(ESlateVisibility::Hidden);
	}
	if (!Border_CheckBox_Visible)
	{
		Img_Border->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UOneKeySkillBox::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Img_Icon)
	{
		Img_Icon->SetBrush(IconBrush);
		Img_Icon->SetDesiredSizeOverride(IconSize);
		Img_Icon->SetRenderTransform(IconRenderTransform);
	}
	if (Text_SkillName)
	{
		FSlateFontInfo FontInfo = Text_SkillName->GetFont();
		FontInfo.Size = Text_SkillFont;
		Text_SkillName->SetFont(FontInfo);
		Text_SkillName->SetJustification(ETextJustify::Left);
	}
	if (!WhiteBG_Visible)
	{
		Img_WhiteBG->SetVisibility(ESlateVisibility::Hidden);
	}
	if (!BlackBG_Visible)
	{
		Img_BlackBG->SetVisibility(ESlateVisibility::Hidden);
	}
	if (!Border_CheckBox_Visible)
	{
		Img_Border->SetVisibility(ESlateVisibility::Hidden);
	}
	Box_Total->SetVisibility(ESlateVisibility::Hidden);
}
