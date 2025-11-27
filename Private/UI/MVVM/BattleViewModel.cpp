// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MVVM/BattleViewModel.h"

#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "GAS/Attributes/RedCCombatSet.h"
#include "GameplayTagsManager.h"
#include "MVVMViewModelBase.h"
#include "Characters/CharacterDefinition.h"
#include "Core/Systems/RedCAssetManager.h"
#include "Core/Systems/RedCItemManager.h"
#include "Elements/Columns/TypedElementAlertColumns.h"
#include "Frameworks/GameState/GameStateArena.h"
#include "GAS/RedCAbilitySet.h"
#include "GAS/Abilities/RedCAttackAbility.h"
#include "GAS/Attributes/RedCCurrencySet.h"
#include "GAS/Attributes/RedCEnergySet.h"
#include "GAS/Attributes/RedCHealthSet.h"
#include "Input/InputConfig.h"
#include "Types/RedCCharacterSkillTypes.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCItemTags.h"
#include "Types/RedCLogChannels.h"
#include "UI/Manager/UIManagerSubsystem.h"
#include "UI/Widget/Battle/Component/ItemBoxWidget.h"



void UBattleViewModel::SetPlayerReference(UWorld* World)
{
	if (ULocalPlayer* LP = World->GetFirstLocalPlayerFromController())
	{
		if (AGameStateArena* GameState = World->GetGameState<AGameStateArena>())
		{
			GS = GameState;
			if (ARedCPlayerController* PlayerController = Cast<ARedCPlayerController>(LP->GetPlayerController(World)))
			{
				PC = PlayerController;
				if (ARedCHeroCharacter* CharacterPlayer = Cast<ARedCHeroCharacter>(PC->GetPawn()))
				{
					Player = CharacterPlayer;
					if (APlayerStateArena* PlayerState = PC->GetPlayerState<APlayerStateArena>())
					{
						PS = PlayerState;
					}
				}
			}
		}
	}
}


void UBattleViewModel::BindToLocalASC(UObject* WorldContext)
{
	UWorld* World = WorldContext->GetWorld();
	if (!World || World->IsNetMode(NM_DedicatedServer)) return;
	
	SetPlayerReference(World);
	
	if (URedCPlayerAbilitySystemComponent* ASC = PS->GetPlayerAbilitySystemComponent())
	{
		// Infomation
		// UE_MVVM_SET_PROPERTY_VALUE = 뷰모델 값이 바뀌면 위젯에 브로드캐스트
		// 아랫줄 바인딩 = 뷰모델 값 변경을 위한 델리게이트 바인딩
		
		// 초기1회 세팅
		PlayerName = PS->CharacterName;
		
		// 게임 시작/종료시점 구독
		if (GS->IsGameStart)
		{
			HandleStartGamePlay(); // GS->OnStartGamePlay 바인딩 전 신호가 왔을경우, GS->IsGameStart로 판단
		}
		else
		{
			GS->OnStartGamePlay.AddUObject(this, &ThisClass::HandleStartGamePlay);	
		}

		// 타워 현황 구독
		InitTowerCounts();
		
		// Attribute 구독 (Gauge)
		UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, ASC->GetNumericAttribute(URedCHealthSet::GetHealthAttribute()));
		ASC->GetGameplayAttributeValueChangeDelegate(URedCHealthSet::GetHealthAttribute()).AddUObject(this, &UBattleViewModel::HandleChangeCurrentHealth);
		UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, ASC->GetNumericAttribute(URedCHealthSet::GetMaxHealthAttribute()));
		ASC->GetGameplayAttributeValueChangeDelegate(URedCHealthSet::GetMaxHealthAttribute()).AddUObject(this, &UBattleViewModel::HandleChangeMaxHealth);
		UE_MVVM_SET_PROPERTY_VALUE(CurrentStamina, ASC->GetNumericAttribute(URedCEnergySet::GetStaminaAttribute()));
		ASC->GetGameplayAttributeValueChangeDelegate(URedCEnergySet::GetStaminaAttribute()).AddUObject(this, &UBattleViewModel::HandleChangeCurrentStamina);
		UE_MVVM_SET_PROPERTY_VALUE(MaxStamina, ASC->GetNumericAttribute(URedCEnergySet::GetMaxStaminaAttribute()));
		ASC->GetGameplayAttributeValueChangeDelegate(URedCEnergySet::GetMaxStaminaAttribute()).AddUObject(this, &UBattleViewModel::HandleChangeMaxStamina);

		// Attribute 구독 (BattleShop)
		UE_MVVM_SET_PROPERTY_VALUE(PlayerGold, ASC->GetNumericAttribute(URedCCurrencySet::GetCoinAttribute()));
		ASC->GetGameplayAttributeValueChangeDelegate(URedCCurrencySet::GetCoinAttribute()).AddUObject(this, &UBattleViewModel::HandleChangeGold);

		// 캐릭터 레벨 구독
		PlayerLevel = PS->CharacterLevel;
		PS->OnCharacterLevelChanged.AddUObject(this, &ThisClass::HandleChangePlayerLevel);

		// 캐릭터 사망 구독
		Player->OnCharacterDead.AddUObject(this, &ThisClass::HandleCharacterDead);
		
		// 캐릭터 스킬 복제 + 쿨타임 구독
		BindPlayerSkills();
		
		// 아이템 복제 + 쿨타임 구독
		BindCharacterItemInfo();
	}
}

#pragma region GamePlayCallback
void UBattleViewModel::SetGamePlayTime()
{
	int32 Init = GS->GetRemainGamePlayTime();
	Server_GameEndTime = (int32)GS->GetServerWorldTimeSeconds() + Init;

	UE_MVVM_SET_PROPERTY_VALUE(RemainGamePlayTime, Init);
	
	GetWorld()->GetTimerManager().SetTimer(
		GamePlayTimeUpdateHandle, this, &ThisClass::UpdateGamePlayTime, 1.0f, true
	);
}

void UBattleViewModel::UpdateGamePlayTime()
{
	if (RemainGamePlayTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(GamePlayTimeUpdateHandle);
		return;
	}
	UE_MVVM_SET_PROPERTY_VALUE(RemainGamePlayTime, RemainGamePlayTime-1);
}


void UBattleViewModel::HandleStartGamePlay()
{
	UE_LOG(LogRedCWidget,Warning, TEXT("HandleStartGamePlay Received"));
	SetGamePlayTime();
	// todo - 게임시작 문구 필요
}


void UBattleViewModel::HandleEndGamePlay()
{
	GetWorld()->GetTimerManager().ClearTimer(GamePlayTimeUpdateHandle);
	RemainGamePlayTime = 0;
	Server_GameEndTime = 0;
}
#pragma endregion

#pragma region AttributeCallback

void UBattleViewModel::HandleChangeCurrentHealth(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, OnAttributeChangeData.NewValue);
}

void UBattleViewModel::HandleChangeMaxHealth(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, OnAttributeChangeData.NewValue);
}

void UBattleViewModel::HandleChangeCurrentStamina(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentStamina, OnAttributeChangeData.NewValue);
}

void UBattleViewModel::HandleChangeMaxStamina(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxStamina, OnAttributeChangeData.NewValue);
}

void UBattleViewModel::HandleChangeGold(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerGold, OnAttributeChangeData.NewValue);
}

#pragma endregion

#pragma region CharacterLevelCallback
void UBattleViewModel::HandleChangePlayerLevel(int32 CharacterLevel)
{
	PlayerLevel = CharacterLevel;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerLevel);
}
#pragma endregion

#pragma region CharacterSkillCallback

void UBattleViewModel::BindPlayerSkills()
{
	TArray<FRedCAbilityInfo> Abilities; 

	const FString& CharID = PS->GetPlayerTravelInfo().CharacterID;
	URedCAbilitySet* AbilitySet = nullptr;

	if (CharID == "Character.Warrior")
	{
		AbilitySet = Player->GetCharacterDefinition()->CharacterAbilitySet;
	}
	else if (CharID == "Character.Archer")
	{
		AbilitySet = Player->GetCharacterDefinition()->CharacterAbilitySet;
	}

	if (!IsValid(AbilitySet))
	{
		UE_LOG(LogRedCWidget, Error, TEXT("BindPlayerSkills: AbilitySet is NULL for %s"), *CharID);
		return;
	}

	Abilities = AbilitySet->GetAbilities(); 
	
	if (URedCPlayerAbilitySystemComponent* ASC = PS->GetPlayerAbilitySystemComponent())
	{
		TArray<FRedCAbilityInfo> AbilityInfos;
		FGameplayTag ParentAbilityTag = InputTags::Input_Action_Ability;
					
		for (auto& ability : Abilities)
		{
			if (!ability.AbilityClass) 
				continue;

			if (!ability.AbilityClass->IsChildOf(URedCAttackAbility::StaticClass()))
				continue;
			
			if (ability.AbilityClass->IsChildOf(URedCAttackAbility::StaticClass()))
			{
				if (URedCAttackAbility* RedCAbility = ability.AbilityClass->GetDefaultObject<URedCAttackAbility>())
				{
					if (!RedCAbility) continue;
					//UE_LOG(LogRedCWidget, Warning, TEXT("ASC Owner -> %s : Available TagName : %s"), *ASC->GetOwner()->GetName(),*RedCAbility->GetAbilityInputTag().ToString())
					if (RedCAbility->GetAbilityInputTag().MatchesTag(ParentAbilityTag))
					{
						CharacterSkillInfo.Add(TPair<FGameplayTag, FCharacterSkillInfo>(
							RedCAbility->GetAbilityInputTag(),
							FCharacterSkillInfo(RedCAbility->GetSkillName(), RedCAbility->GetCoolDownDuration())));
					}
					ASC->RegisterGameplayTagEvent(RedCAbility->GetAbilityInputTag(), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnSkillTagChanged);
				}
			}
		}
	}
}


void UBattleViewModel::OnSkillTagChanged(FGameplayTag GameplayTag, int onCount)
{
	//UE_LOG(LogRedCWidget, Warning, TEXT("SkillCoolTime Called, Tag : %s / onCount : %d"), *GameplayTag.ToString(), onCount);
	TArray<FString> Tokens;
	GameplayTag.GetTagName().ToString().ParseIntoArray(Tokens, TEXT("."));
	for (auto k :  CharacterSkillInfo)
	{
		if (k.Key == GameplayTag)
		{
			if (onCount==1)
			{
				OnStartedSkillCoolTime.Broadcast(ECharacterAttackTypeHelper::StringToAttackType(Tokens.Last()), k.Value.Cooldown);
				return;
			}
			else if (onCount==0)
			{
				//OnStartedSkillCoolTime.Clear();
				//OnEndItemCoolTime.Broadcast(ItemID);
			}
		}
	}
	
}
#pragma endregion

#pragma region CharacterDeath

void UBattleViewModel::HandleCharacterDead(int32 InputRespawnCountDown)
{
	UE_LOG(LogRedCWidget,Warning, TEXT("InputRespawnCountDown Received"));

	UE_MVVM_SET_PROPERTY_VALUE(RespawnCountDown, InputRespawnCountDown);
	
	GetWorld()->GetTimerManager().SetTimer(
		RespawnTimeUpdateHandle, this, &ThisClass::UpdateRespawnTime, 1.0f, true
	);
}

void UBattleViewModel::UpdateRespawnTime()
{
	if (RespawnCountDown <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimeUpdateHandle);
		return;
	}
	UE_MVVM_SET_PROPERTY_VALUE(RespawnCountDown, RespawnCountDown-1);
}

#pragma endregion

#pragma region CharacterItemCallback

void UBattleViewModel::BindCharacterItemInfo()
{
	// Local PS에 Item복제될때까지 시도
	if (PS->CharacterItemInfo.Items.Num()<=0)
	{
		GetWorld()->GetTimerManager().SetTimer(PlayerItemInitHandle, [this]()
		{
			if (PS->CharacterItemInfo.Items.Num()>0)
			{
				CharacterItemInfo = PS->CharacterItemInfo;
				UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CharacterItemInfo);
				GetWorld()->GetTimerManager().ClearTimer(PlayerItemInitHandle);

				BindItemCoolTime();
			}
		},
		0.1f,
		true
		);
	}
	else
	{
		CharacterItemInfo = PS->CharacterItemInfo;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CharacterItemInfo);
		
		BindItemCoolTime();
	}

	PS->CharacterItemInfo.OnItemLevelChanged.AddUObject(this, &ThisClass::HandleChangePlayerItem);
}

void UBattleViewModel::BindItemCoolTime()
{
	// 가지고 있는 모든 아이템 쿨타임 바인딩
	if (URedCPlayerAbilitySystemComponent* ASC = PS->GetPlayerAbilitySystemComponent())
	{
		FGameplayTag ParentCooldownTag = ItemCoolDown::Cooldown_Item;
		for (auto Item : CharacterItemInfo.Items)
		{
			if (Item.ItemID != NAME_None)
			{
				FString Tag = ParentCooldownTag.ToString().Append(".")+Item.ItemID.ToString();
				FGameplayTag ItemCoolDownTag = FGameplayTag::RequestGameplayTag(FName(*Tag));
				ASC->RegisterGameplayTagEvent(ItemCoolDownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCooldownTagChanged);
			}
		}
	}
}


void UBattleViewModel::OnCooldownTagChanged(FGameplayTag GameplayTag, int Count)
{
	TArray<FString> Tokens;
	GameplayTag.GetTagName().ToString().ParseIntoArray(Tokens, TEXT("."));
	FName ItemID = FName(*Tokens.Last());

	UE_LOG(LogRedCWidget, Warning, TEXT("Cooldown effect changed, ItemID : %s / Count : %d"), *ItemID.ToString(), Count);
		
	if (Count==1)
	{
		OnStartedItemCoolTime.Broadcast(ItemID);
	}
	else if (Count==0)
	{
		OnEndItemCoolTime.Broadcast(ItemID);
	}
}


void UBattleViewModel::HandleChangePlayerItem(FName ItemID, int32 ItemLevel)
{
	FRedCItemInfo* UpdateItem = nullptr;

	for (FRedCItemInfo& FindItem : CharacterItemInfo.Items)
	{
		if (FindItem.ItemID == ItemID)
		{
			UpdateItem = &FindItem;
			break;
		}
	}

	if (UpdateItem)
	{
		UpdateItem->ItemID = ItemID;
		UpdateItem->ItemLevel = ItemLevel;
	}
	
	OnChangedItemLevel.Broadcast(UpdateItem->ItemID, UpdateItem->ItemLevel);
}

#pragma endregion

#pragma region Tower


void UBattleViewModel::InitTowerCounts()
{
	const uint8 TeamID = PS->GetGenericTeamId().GetId();

	if (TeamID == 1)
	{
		MyTeamTowerCount = GS->GetRedTeamTowerNum();
		EnemyTeamTowerCount = GS->GetBlueTeamTowerNum();
	}
	else if (TeamID == 0)
	{
		MyTeamTowerCount = GS->GetBlueTeamTowerNum();
		EnemyTeamTowerCount = GS->GetRedTeamTowerNum();
	}

	GS->OnRedTeamTowerDestroyed.AddUObject(this, &ThisClass::HandleRedTeamTowerDestroyed);
	GS->OnBlueTeamTowerDestroyed.AddUObject(this, &ThisClass::HandleBlueTeamTowerDestroyed);
}


void UBattleViewModel::HandleRedTeamTowerDestroyed()
{
	const uint8 TeamID = PS->GetGenericTeamId().GetId();

	if (TeamID == 1)
	{
		const int32 NewValue = MyTeamTowerCount - 1;
		UE_MVVM_SET_PROPERTY_VALUE(MyTeamTowerCount, NewValue);
	}
	else
	{
		const int32 NewValue = EnemyTeamTowerCount - 1;
		UE_MVVM_SET_PROPERTY_VALUE(EnemyTeamTowerCount, NewValue);
	}
}

void UBattleViewModel::HandleBlueTeamTowerDestroyed()
{
	const uint8 TeamID = PS->GetGenericTeamId().GetId();

	if (TeamID == 0)
	{
		const int32 NewValue = MyTeamTowerCount - 1;
		UE_MVVM_SET_PROPERTY_VALUE(MyTeamTowerCount, NewValue);
	}
	else
	{
		const int32 NewValue = EnemyTeamTowerCount - 1;
		UE_MVVM_SET_PROPERTY_VALUE(EnemyTeamTowerCount, NewValue);
	}
}

#pragma endregion