// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Frameworks/RedCGameStateBase.h"
#include "PlayerMatchingViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UPlayerMatchingViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void BindToLocalWorld(UObject* WorldContext);
	void HandlePressedCountChanged(int32 NewCount);
	void HandleMatchingCompleted(bool NewBool);
	void HandleMatchingCountDownChanged(int32 NewCount);
	void HandleMatchingCharacterSelectionCountDownChanged(int NewCount);
	
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	int32 PressedCount =0;

	UPROPERTY(BlueprintReadOnly,FieldNotify)
	int32 PlayerTarget =0;

	UPROPERTY(BlueprintReadOnly,FieldNotify)
	int32 LobbyCountDown=0;

	UPROPERTY(BlueprintReadOnly,FieldNotify)
	int32 CharacterSelectionCountDown=0;

	UPROPERTY(BlueprintReadOnly,FieldNotify)
	bool MatchingCompleted = false;

};
