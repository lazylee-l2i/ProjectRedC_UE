// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/Component/TwoKeySkillBox.h"

#include "Components/Image.h"


void UTwoKeySkillBox::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Img_Icon)
	{
		Img_Icon->SetBrush(IconBrush);
		Img_Icon->SetDesiredSizeOverride(IconSize);
		Img_Icon->SetRenderTransform(IconRenderTransform);
	}
	if (Img_Icon_2)
	{
		Img_Icon_2->SetBrush(IconBrush_2);
		Img_Icon_2->SetDesiredSizeOverride(IconSize_2);
		Img_Icon_2->SetRenderTransform(IconRenderTransform_2);
	}
	if (Text_SkillName)
	{
		FSlateFontInfo FontInfo = Text_SkillName->GetFont();
		FontInfo.Size = Text_SkillFont;
		Text_SkillName->SetFont(FontInfo);
		Text_SkillName->SetJustification(ETextJustify::Left);
	}
	if (!BlackBG_Visible)
	{
		Img_BlackBG->SetVisibility(ESlateVisibility::Hidden);
	}
	if (SkillCoolDownText)
	{
		SkillCoolDownText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UTwoKeySkillBox::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Img_Icon)
	{
		Img_Icon->SetBrush(IconBrush);
		Img_Icon->SetDesiredSizeOverride(IconSize);
		Img_Icon->SetRenderTransform(IconRenderTransform);
	}
	if (Img_Icon_2)
	{
		Img_Icon_2->SetBrush(IconBrush_2);
		Img_Icon_2->SetDesiredSizeOverride(IconSize_2);
		Img_Icon_2->SetRenderTransform(IconRenderTransform_2);
	}
	if (Text_SkillName)
	{
		FSlateFontInfo FontInfo = Text_SkillName->GetFont();
		FontInfo.Size = Text_SkillFont;
		Text_SkillName->SetFont(FontInfo);
		Text_SkillName->SetJustification(ETextJustify::Left);
	}
	if (!BlackBG_Visible)
	{
		Img_BlackBG->SetVisibility(ESlateVisibility::Hidden);
	}
	if (SkillCoolDownText)
	{
		SkillCoolDownText->SetVisibility(ESlateVisibility::Hidden);
	}
}
