// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#define LOG_ON_OFF 1

namespace LogHelper
{
	/*
	* Outputs InMessage using UE_LOG.
	* If Actor is not nullptr, checks the actor's HasAuthority to indicate whether it's being called from Server or Client.
	* If bAddOnScreen is true, outputs the message to screen via AddOnScreenDebugMessage call.
	*/
	static void Print(const FLogCategoryBase& LogCategory, const ELogVerbosity::Type LogVerbosity,
		const FString& LogMessage, const AActor* Actor = nullptr, const bool bAddOnScreen = false)
	{
		if (!LOG_ON_OFF)
		{
			return;
		}
		
		FColor Color = FColor::Yellow;
		FString Message = FString();
		
		if (Actor)
		{
			if (Actor->HasAuthority())
			{
				Color = FColor::Magenta;
				Message.Append(FString::Printf(TEXT("[S 0] %s : "), *GetNameSafe(Actor)));
			}
			else
			{
				Color = FColor::Cyan;
				const int32 EditorID = GPlayInEditorID;
				Message.Append(FString::Printf(TEXT("[C %d] %s : "), EditorID, *GetNameSafe(Actor)));
			}
		}

		Message.Append(LogMessage);

		if (bAddOnScreen)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, Message);
		}

		FMsg::Logf_Internal(__FILE__, __LINE__, LogCategory.GetCategoryName(), LogVerbosity, TEXT("%s"), *Message);
	}
}
