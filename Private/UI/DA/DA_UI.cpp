// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DA/DA_UI.h"

#include "Types/RedCLogChannels.h"
#include "UI/Widget/_Base/BaseInGameHUDWidget.h"
#include "UI/Widget/_Base/BaseLoadingWidget.h"
#include "UI/Widget/_Base/BaseMenuWidget.h"

TSubclassOf<UBaseLoadingWidget> UDA_UI::GetLoadingWidgetClass() const
{
	if (LoadingHUDClass)
	{
		return LoadingHUDClass.Get(); 
	}
	return nullptr;
}

TSubclassOf<UBaseIntro> UDA_UI::GetIntroWidgetClass() const
{
	if (IntroHUDClass)
	{
		return IntroHUDClass.Get(); 
	}
	return nullptr; 
}

TSubclassOf<UBaseInGameHUDWidget> UDA_UI::GetInGameHUDClass(EProjectUIType Mode) const
{
	if (const TSubclassOf<UBaseInGameHUDWidget>* Found = ModeHUDClasses.Find(Mode))
	{
		if (*Found)            
		{
			return *Found;
		}
	}
	return nullptr;  
}

TSubclassOf<UBaseMenuWidget> UDA_UI::GetMenuHUDClass(EProjectMenuType Type) const
{
	if (const TSubclassOf<UBaseMenuWidget>* Found = MenuHUDClasses.Find(Type))
	{
		if (*Found)            
		{
			return *Found;
		}
	}
	return nullptr;  
}
