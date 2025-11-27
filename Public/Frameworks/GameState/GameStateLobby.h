// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/RedCPlayerController.h"
#include "Frameworks/RedCGameStateBase.h"
#include "Types/RedCCharacterSelectionInfo.h"
#include "GameStateLobby.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPressedCountChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchingLobbyCountDownTicking, int32);
DECLARE_MULTICAST_DELEGATE(FOnMatchingLobbyCountDownFinished);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchingCharacterSelectionCountDownTicking, int32);
DECLARE_MULTICAST_DELEGATE(FOnMatchingCharacterSelectionCountDownFinished);


/**
 * 
 */
UCLASS()
class PROJECTREDC_API AGameStateLobby : public ARedCGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnPressedCountChanged OnPressedCountChanged;
	FOnMatchingLobbyCountDownTicking OnMatchingCountDownTicking;
	FOnMatchingLobbyCountDownFinished OnMatchingCountDownFinished;
	FOnMatchingCharacterSelectionCountDownTicking OnMatchingCharacterSelectionCountDownTicking;
	FOnMatchingCharacterSelectionCountDownFinished OnMatchingCharacterSelectionCountDownFinished;
	
	FTimerHandle TimerHandle;
	
	UPROPERTY(ReplicatedUsing = OnRep_PressedCount)
	int32 PressedCount = 0;

	// 로비 카운트다운 + 캐릭터선택창 진입 안내문구
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_LobbyCountDown, Category="GameRule")
	int32 MatchingLobbyCountDown = 3;
	int32 ResetLobbyCountDown = 3;
	int32 ShowMatchStartComment = 1;
	// 캐릭터 선택창 카운트다운
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_CharacterSelectionCountDown, Category="GameRule")
	int32 MatchingCharacterSelectionCountDown = 5;
	int32 ResetCharacterSelectionCountDown = 5;
	int32 ShowGameStartComment = 1;

	// 유팀 관련
	UPROPERTY(Replicated)
	bool MatchStarted = false;
	void MakeTeam(TArray<TWeakObjectPtr<ARedCPlayerController>> PlayerControllers);
	
	UPROPERTY(Replicated)
	TArray<FRedCCharacterSelectionInfo> MatchedPlayerInfo;
	
public:
	void StartLobbyCountDown();
	void HandleLobbyCountDown();
	void StartCharacterSelectionCountDown();
	void HandleCharacterSelectionCountDown();

private:
	UFUNCTION()
	void OnRep_PressedCount();
	UFUNCTION()
	void OnRep_LobbyCountDown();
	UFUNCTION()
	void OnRep_CharacterSelectionCountDown();

	
};
