// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Frameworks/RedCPlayerStateBase.h"
#include "Types/RedCItemInfoTable.h"
#include "PlayerStateArena.generated.h"

struct FOnAttributeChangeData;
class URedCCurrencySet;
class URedCCombatSet;
class URedCMovementSet;
class URedCEnergySet;
class URedCHealthSet;
class URedCPlayerAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPrivatePlayerNameChanged, FName);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterNameChanged, FName);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterLevelChanged, int32);

/**
 * 
 */
UCLASS()
class PROJECTREDC_API APlayerStateArena :	public ARedCPlayerStateBase,
											public IAbilitySystemInterface,
											public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	APlayerStateArena();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void InitializeCharacterItems(FString CharacterID);
	
	// Delegate
	FOnPrivatePlayerNameChanged OnPrivatePlayerNameChanged;
	FOnCharacterNameChanged OnCharacterNameChanged;
	FOnCharacterLevelChanged OnCharacterLevelChanged;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URedCPlayerAbilitySystemComponent> PlayerAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<URedCHealthSet> PlayerHealthSet;

	UPROPERTY()
	TObjectPtr<URedCEnergySet> PlayerEnergySet;

	UPROPERTY()
	TObjectPtr<URedCMovementSet> PlayerMovementSet;

	UPROPERTY()
	TObjectPtr<URedCCombatSet> PlayerCombatSet;

	UPROPERTY()
	TObjectPtr<URedCCurrencySet> PlayerCurrencySet;

public:
	void BroadcastPlayerInfo();
	void SetPrivatePlayerName(const FName NewPlayerName);
	
	FRedCItemInfo* FindItemInfo(FName ItemID);
	FName FindItemID(int32 InputKey);
	
	// 캐릭터 정보
	UPROPERTY(ReplicatedUsing=OnRep_CharacterName)
	FName CharacterName;
	
	UPROPERTY(ReplicatedUsing=OnRep_CharacterLevel)
	int32 CharacterLevel;
	
	// 캐릭터 아이템 정보
	UPROPERTY(Replicated)
	FRedCItemArray CharacterItemInfo;
	
	int32 MaxItemNumber = 16;

private:
	UFUNCTION()
	void OnRep_PrivatePlayerName();

	UFUNCTION()
	void OnRep_CharacterName();
	
	UFUNCTION()
	void OnRep_CharacterLevel();
	
	UPROPERTY(ReplicatedUsing=OnRep_PrivatePlayerName)
	FName PrivatePlayerName;

#pragma region Getter
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	URedCPlayerAbilitySystemComponent* GetPlayerAbilitySystemComponent() const;

	URedCHealthSet* GetRedCHealthSet() const;
	URedCEnergySet* GetRedCEnergySet() const;
	URedCMovementSet* GetRedCMovementSet() const;
	URedCCombatSet* GetRedCCombatSet() const;
	URedCCurrencySet* GetRedCCurrencySet() const;

	FName GetPrivatePlayerName() const { return PrivatePlayerName; }
	
#pragma endregion

#pragma region TeamAssign
private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	
public:
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override { TeamId = FGenericTeamId(TeamID.GetId()); }
	
#pragma endregion 
};
