// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/_Base/Component/BaseScreenFadeIn.h"


void UBaseScreenFadeIn::PlayFadeInAnim()
{
	if (Img_FadeIn)
	{
		PlayAnimation(FadeInAnim, 0, 1, EUMGSequencePlayMode::Forward, 1);
	}
}
