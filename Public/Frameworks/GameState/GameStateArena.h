// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AICharacter/RedCAI_Tower.h"
#include "Controllers/RedCPlayerController.h"
#include "Environment/BaseStarterWall.h"
#include "Frameworks/RedCGameStateBase.h"
#include "Frameworks/GameMode/GameModeArena.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "Types/RedCAbilityTypes.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GameStateArena.generated.h"

class ARedCAISpawnActor;
class ARedCCharacter;
struct FGenericTeamId;

DECLARE_MULTICAST_DELEGATE(FOnStartGamePlay)
DECLARE_MULTICAST_DELEGATE(FOnBlueTeamTowerDestroyed)
DECLARE_MULTICAST_DELEGATE(FOnRedTeamTowerDestroyed)
DECLARE_MULTICAST_DELEGATE(FOnWidgetEndLoading)
/**
 * 
 */
UCLASS()
class PROJECTREDC_API AGameStateArena : public ARedCGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FOnStartGamePlay OnStartGamePlay;
	FOnBlueTeamTowerDestroyed OnBlueTeamTowerDestroyed;
	FOnRedTeamTowerDestroyed OnRedTeamTowerDestroyed;
	FOnWidgetEndLoading OnWidgetEndLoading;

public:
	// Getter
	UFUNCTION()
	int32 GetRemainGamePlayTime() const;
	TArray<APlayerController*> GetConnectedPlayers();
	TArray<ARedCAI_Tower*> GetRedTeamTowers();
	TArray<ARedCAI_Tower*> GetBlueTeamTowers();
	int GetBlueTeamTowerNum();
	int GetRedTeamTowerNum();
	TArray<ABaseStarterWall*> GetStarterWalls();
	// Setter
	void AddConnectedPlayer(APlayerController* PlayerController);
	void AddBlueTeamTowerArray(ARedCAI_Tower* BlueTeamTower);
	void AddRedTeamTowerArray(ARedCAI_Tower* RedTeamTower);
	void AddStarterWall(ABaseStarterWall* Wall);
	void AddAISpawners(ARedCAISpawnActor* AISpawn);
	
public:
	// 게임 시작 관련 함수
	void GameStart();
	void UpdateTeamPlayerCache(AGameModeArena* GM);
	void UpdateGamePlayTime();
	UPROPERTY(ReplicatedUsing=OnRep_IsGameStart)
	bool IsGameStart = false;
	UFUNCTION()
	void OnRep_IsGameStart();
	bool IsGameEnd = false;
	
private:
	// 게임 플레이 타임 설정
	FTimerHandle GamePlayTimeTimerHandle;
	UPROPERTY(Getter = GetRemainGamePlayTime, EditDefaultsOnly)
	int32 RemainGamePlayTime = 600;

private:
	UPROPERTY()
	TArray<ABaseStarterWall*> StarterWalls;

	UPROPERTY()
	TArray<ARedCAISpawnActor*> AISpawners;
	
#pragma region Give Coin
public:
	void GiveCoinHandle(AActor* GivenActor, AActor* DeadActor);
	void GivePersonalCoin(AActor* GivenActor, AActor* DeadActor, EActorType ActorType);
	void GiveTeamCoin(ETeamType TeamId, float CoinAmount = 0);

private:
	UPROPERTY()
	mutable TMap<AActor*, UAbilitySystemComponent*> CachedASCMap;

	void ApplyCoinEffect(AActor* GivenActor, float CoinAmount);
	
	UAbilitySystemComponent* TryGetActorASCInMap(AActor* Actor) const;
	
	FGameplayEffectSpecHandle GenerateCoinEffectHandle(AActor* GivenActor);

	void GiveGlobalTimeCoin();
#pragma endregion

#pragma region Tower Destroy Handle
private:
	UPROPERTY()
	TArray<ARedCAI_Tower*> BlueTeamTowers;
	UPROPERTY()
	TArray<ARedCAI_Tower*> RedTeamTowers;
	
	UPROPERTY(ReplicatedUsing=OnRep_BlueTowerCount)
	int32 BlueTowerCount = 0;
	UPROPERTY(ReplicatedUsing=OnRep_RedTowerCount)
	int32 RedTowerCount = 0;
	
	UFUNCTION()
	void OnRep_BlueTowerCount();
	UFUNCTION()
	void OnRep_RedTowerCount();
	
	UFUNCTION()
	void HandleDestroyTower(AActor* DestroyedTower);
	void HandleGameEndUI(ETeamType WinnerTeam);
	void CheckAllTowerDestroyed();

	// Team Assign
	UPROPERTY()
	TArray<APlayerController*> ConnectedPlayers;
	int8 GetWinnerTeamInt(ETeamType Team);
#pragma endregion
};