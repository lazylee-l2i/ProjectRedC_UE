// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "Core/Systems/RedCItemManager.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "GAS/Attributes/RedCCombatSet.h"
#include "GAS/Attributes/RedCCurrencySet.h"
#include "GAS/Attributes/RedCEnergySet.h"
#include "GAS/Attributes/RedCHealthSet.h"
#include "GAS/Attributes/RedCMovementSet.h"
#include "Net/UnrealNetwork.h"
#include "Types/RedCLogChannels.h"


APlayerStateArena::APlayerStateArena()
{
	PlayerAbilitySystemComponent = CreateDefaultSubobject<URedCPlayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	PlayerAbilitySystemComponent->SetIsReplicated(true);
	PlayerAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PlayerHealthSet = CreateDefaultSubobject<URedCHealthSet>(TEXT("HealthSet"));
	PlayerEnergySet = CreateDefaultSubobject<URedCEnergySet>(TEXT("EnergySet"));
	PlayerMovementSet = CreateDefaultSubobject<URedCMovementSet>(TEXT("MovementSet"));
	PlayerCombatSet = CreateDefaultSubobject<URedCCombatSet>(TEXT("CombatSet"));
	PlayerCurrencySet = CreateDefaultSubobject<URedCCurrencySet>(TEXT("CurrencySet"));

	TeamId = FGenericTeamId::NoTeam;

	// 아이템 배열 Init
	if (GetNetMode() == NM_DedicatedServer)
	{
		CharacterItemInfo.Items.SetNum(MaxItemNumber);
		for (int i =0; i<MaxItemNumber; i++)
		{
			CharacterItemInfo.Items[i] = FRedCItemInfo();
		}	
	}
}

void APlayerStateArena::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerStateArena, CharacterItemInfo);
	DOREPLIFETIME(APlayerStateArena, TeamId);
	
	DOREPLIFETIME(APlayerStateArena, CharacterLevel);
	DOREPLIFETIME(APlayerStateArena, CharacterName);

	DOREPLIFETIME(APlayerStateArena, PrivatePlayerName);
}

void APlayerStateArena::InitializeCharacterItems(FString CharacterID)
{
	// ItemManager 가져와서 해당 데이터 찾고 CharacterItemInfo에 주입
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (URedCItemManager* ItemManager = GI->GetSubsystem<URedCItemManager>())
		{
			//TArray<TPair<EItemType, FName>> CharacterItemRowName = ItemManager->GetHeroItemSet(FCString::Atoi(*PlayerTravelInfo.CharacterID));
			TArray<TPair<EItemType, FName>> CharacterItemRowName = ItemManager->GetHeroItemSet(CharacterID);
			for (int i=0; i<CharacterItemRowName.Num(); i++)
			{
				if (FRedCItemInfoTable* FindItem = ItemManager->FindItem(CharacterItemRowName[i].Key, CharacterItemRowName[i].Value))
				{
					FRedCItemInfo ItemInfo = ItemManager->ParseDataTableToItemInfo(CharacterItemRowName[i].Value, FindItem);

					int32 InputSlot = ItemManager->GetItemSlotArray(CharacterItemRowName[i].Key);
					
					if (InputSlot != -1)
					{
						CharacterItemInfo.Items[InputSlot] = ItemInfo;
						//ItemManager->ItemIDArray.Add({ ItemInfo.ItemID, CharacterItemRowName[i].Key });
					}
					else
					{
						UE_LOG(LogRedCWidget, Warning, TEXT("Already Exist Slot : %d"), InputSlot);
					}
				}
			}
			CharacterItemInfo.MarkArrayDirty();
		}
	}
}

void APlayerStateArena::OnRep_PrivatePlayerName()
{
	OnPrivatePlayerNameChanged.Broadcast(PrivatePlayerName);
}

void APlayerStateArena::OnRep_CharacterName()
{
	OnCharacterNameChanged.Broadcast(CharacterName);
}

void APlayerStateArena::OnRep_CharacterLevel()
{
	OnCharacterLevelChanged.Broadcast(CharacterLevel);
}

void APlayerStateArena::BroadcastPlayerInfo()
{
	OnRep_PrivatePlayerName();
	OnRep_CharacterName();
	OnRep_CharacterLevel();
}

void APlayerStateArena::SetPrivatePlayerName(const FName NewPlayerName)
{
	PrivatePlayerName = NewPlayerName;
}

#pragma region Getter
UAbilitySystemComponent* APlayerStateArena::GetAbilitySystemComponent() const
{
	return GetPlayerAbilitySystemComponent();
}

URedCPlayerAbilitySystemComponent* APlayerStateArena::GetPlayerAbilitySystemComponent() const
{
	return PlayerAbilitySystemComponent;
}

URedCHealthSet* APlayerStateArena::GetRedCHealthSet() const
{
	return PlayerHealthSet;
}

URedCEnergySet* APlayerStateArena::GetRedCEnergySet() const
{
	return PlayerEnergySet;
}

URedCMovementSet* APlayerStateArena::GetRedCMovementSet() const
{
	return PlayerMovementSet;
}

URedCCombatSet* APlayerStateArena::GetRedCCombatSet() const
{
	return PlayerCombatSet;
}

URedCCurrencySet* APlayerStateArena::GetRedCCurrencySet() const
{
	return PlayerCurrencySet;
}

#pragma endregion

FRedCItemInfo* APlayerStateArena::FindItemInfo(FName ItemID)
{
	for (FRedCItemInfo& FindItem : CharacterItemInfo.Items)
	{
		if (FindItem.ItemID == ItemID)
		{
			return &FindItem;
		}
	}
	
	return nullptr;
}

FName APlayerStateArena::FindItemID(int32 InputKey)
{
	//UE_LOG(LogRedCWidget, Warning, TEXT("Input Key : %d"), InputKey);
	if (CharacterItemInfo.Items.IsValidIndex(InputKey-1))
	{
		return CharacterItemInfo.Items[InputKey-1].ItemID;
	}

	return NAME_None;
}
