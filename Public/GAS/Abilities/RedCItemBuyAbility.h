// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "GAS/Abilities/RedCGameplayAbility.h"
#include "Types/RedCItemTypes.h"
#include "RedCItemBuyAbility.generated.h"

class APlayerStateArena;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCItemBuyAbility : public URedCGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Coin")
	TSubclassOf<UGameplayEffect> CoinDeltaGE;

	UPROPERTY(EditDefaultsOnly, Category="Stat")
	TSubclassOf<UGameplayEffect> ItemMaxStatGE;
	
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	TSubclassOf<UGameplayEffect> ItemStatGE;

	UPROPERTY(EditDefaultsOnly, Category="Stat")
	TSubclassOf<UGameplayEffect> GE_ItemHealPotionStat;
	
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	TSubclassOf<UGameplayEffect> GE_ItemBuffPotionStat;
	
	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	TSubclassOf<UGameplayEffect> ItemCooldownGE;


public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
public:
	FGameplayTag MakeCoolTimeTagByRowName(FName ItemRowName);


	void ApplyItemCooldown(URedCPlayerAbilitySystemComponent* ASC, FGameplayTag CooldownTag, int32 ItemCooldown);
	void ApplyItemStat_MaxGauge(URedCPlayerAbilitySystemComponent* Asc, float MaxHealth);
	void ApplyItemStat(URedCPlayerAbilitySystemComponent* ASC, FRedCItemInfo* Item);
	void ApplyCoinDecrease(URedCPlayerAbilitySystemComponent* ASC, int32 Price);
	void ApplyHealItemStat(URedCPlayerAbilitySystemComponent* ASC, FRedCItemInfo* Item);
	void ApplyBuffItemStat(URedCPlayerAbilitySystemComponent* ASC, FRedCItemInfo* Item);
	UFUNCTION()
	void ItemBuyEvent(FGameplayEventData Payload);
	
	
	bool CheckValidAbility(URedCPlayerAbilitySystemComponent* ASC,FGameplayTag CooldownTag);
	FRedCItemInfo* CheckValidItem(APlayerStateArena* PS, FName ItemID);
	int32 CalcNextPrice(const FRedCItemInfo& Item, int32& NextItemPrice);
	bool CheckValidCoin(URedCPlayerAbilitySystemComponent* ASC, int32 Price);
	
};
