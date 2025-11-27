// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Characters/RedCHeroCharacter.h"
#include "Frameworks/RedCGameStateBase.h"
#include "Frameworks/GameState/GameStateArena.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GAS/Attributes/RedCAttributeSet.h"
#include "Types/RedCCharacterSkillTypes.h"
#include "Types/RedCItemTypes.h"
#include "UI/Widget/Battle/Component/ItemBoxWidget.h"
#include "BattleViewModel.generated.h"

class ARedCPlayerController;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangedItemLevel, FName , int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartedItemCoolTime, FName)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEndItemCoolTime, FName)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStartedSkillCoolTime, ECharacterAttackType, int32)
DECLARE_MULTICAST_DELEGATE(FCharacterAbiltiyReplicated)

UCLASS(BlueprintType)
class PROJECTREDC_API UBattleViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	// Delegate
	FOnChangedItemLevel OnChangedItemLevel;
	FOnStartedItemCoolTime OnStartedItemCoolTime;
	FOnEndItemCoolTime OnEndItemCoolTime;
	FOnStartedSkillCoolTime OnStartedSkillCoolTime;
	FCharacterAbiltiyReplicated OnCharacterAbiltiyReplicated;

public:
	UFUNCTION(BlueprintCallable)
	void BindToLocalASC(UObject* WorldContext);
	void SetPlayerReference(UWorld* World);

	UPROPERTY()
	ARedCPlayerController* PC = nullptr;
	UPROPERTY()
	ARedCHeroCharacter* Player = nullptr;
	UPROPERTY()
	APlayerStateArena* PS = nullptr;
	UPROPERTY()
	AGameStateArena* GS = nullptr;

	// 게임시작
	void HandleStartGamePlay();
	void SetGamePlayTime();
	void UpdateGamePlayTime();

	// 게임종료
	void HandleEndGamePlay();

	// 스킬 바인딩
	void BindPlayerSkills();
	void OnSkillTagChanged(FGameplayTag GameplayTag, int onCount);
	
	/** View */
	// Game
	FTimerHandle GamePlayTimeUpdateHandle;
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	int32 RemainGamePlayTime=0;
	int32 Server_GameEndTime=0;

	// Death
	FTimerHandle RespawnTimeUpdateHandle;
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	int32 RespawnCountDown=0;
	void HandleCharacterDead(int32 InputRespawnCountDown);
	void UpdateRespawnTime();

	// Gauge
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	float CurrentHealth=0;
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	float MaxHealth=0;
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	float CurrentStamina=0;
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	float MaxStamina=0;

	// BattleShop
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	int32 PlayerGold=0;
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	FName PlayerName="None";
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	int32 PlayerLevel=0;
	UPROPERTY(BlueprintReadOnly,FieldNotify)
	FRedCItemArray CharacterItemInfo;

	// Skill
	TArray<TPair<FGameplayTag, FCharacterSkillInfo>> CharacterSkillInfo;

	// Tower
	UPROPERTY(BlueprintReadWrite, FieldNotify)
	int MyTeamTowerCount;
	UPROPERTY(BlueprintReadWrite, FieldNotify)
	int EnemyTeamTowerCount;
	void InitTowerCounts();
	
	/** CharacterItem Callback Function */
	void BindItemCoolTime();
	FTimerHandle PlayerItemInitHandle;
	FTimerHandle PlayerSkillInitHandle;
	
	void BindCharacterItemInfo();
	void HandleChangePlayerLevel(int32 CharacterLevel);
	void OnCooldownTagChanged(FGameplayTag GameplayTag, int Count);
	
	/** Attribute Callback Function */
public:
	void HandleChangeCurrentHealth(const FOnAttributeChangeData& OnAttributeChangeData);
	void HandleChangeMaxHealth(const FOnAttributeChangeData& OnAttributeChangeData);
	void HandleChangeCurrentStamina(const FOnAttributeChangeData& OnAttributeChangeData);
	void HandleChangeMaxStamina(const FOnAttributeChangeData& OnAttributeChangeData);
	void HandleChangeGold(const FOnAttributeChangeData& OnAttributeChangeData);
	void HandleChangePlayerItem(FName ItemID, int32 ItemLevel);
	void HandleRedTeamTowerDestroyed();
	void HandleBlueTeamTowerDestroyed();
};
