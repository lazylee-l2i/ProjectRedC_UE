// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/_Base/Component/BaseSkillBox.h"

#include <ranges>

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Types/RedCLogChannels.h"
#include "UI/Widget/_Base/Component/BaseBattleTextBlock.h"


void UBaseSkillBox::StartCoolTime(int32 CooldownTime)
{
	if (CooldownTime <= 0)	return;
	
	float AnimLen = SkillCoolDownAnim->GetEndTime();
	float PlayRate = AnimLen/CooldownTime;

	Img_GrayBG->SetVisibility(ESlateVisibility::Visible);
	Img_Cooldown->SetVisibility(ESlateVisibility::Visible);
	SkillCoolDownText->SetVisibility(ESlateVisibility::Visible);
	RemainingSeconds = (float)CooldownTime;
	SkillCoolDownText->SetText(FText::AsNumber(RemainingSeconds));
	PlayAnimation(SkillCoolDownAnim, 0, 1, EUMGSequencePlayMode::Forward, PlayRate);

	EndDelegate.Clear();
	EndDelegate.BindDynamic(this, &ThisClass::EndSkillCoolDown);
	BindToAnimationFinished(SkillCoolDownAnim, EndDelegate);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CooldownTimerHandle);
		World->GetTimerManager().SetTimer(
			CooldownTimerHandle, this, &ThisClass::UpdateCooldownText, 0.1f, true, 0.f
		);
	}
}

void UBaseSkillBox::UpdateCooldownText()
{
	// 남은 시간 감소 및 갱신
	RemainingSeconds = RemainingSeconds-0.1f;;
	if (RemainingSeconds > 0 && RemainingSeconds > 3)
	{
		SkillCoolDownText->SetText(FText::AsNumber((int)RemainingSeconds));
	}
	else if (RemainingSeconds > 0)
	{
		FNumberFormattingOptions FormatOptions;
		FormatOptions.MinimumFractionalDigits = 1; 
		FormatOptions.MaximumFractionalDigits = 1;

		SkillCoolDownText->SetText(FText::AsNumber(RemainingSeconds, &FormatOptions));
	}
	else
	{
		SkillCoolDownText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBaseSkillBox::EndSkillCoolDown()
{
	UE_LOG(LogRedCWidget,Warning,TEXT("Skill CoolDown End"));
	Img_GrayBG->SetVisibility(ESlateVisibility::Hidden);
	Img_Cooldown->SetVisibility(ESlateVisibility::Hidden);
	SkillCoolDownText->SetVisibility(ESlateVisibility::Hidden);

	PlayAnimation(CoolDownFinished, 0, 1, EUMGSequencePlayMode::Forward);
}

