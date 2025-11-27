// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/RedCItemBuyAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Characters/RedCCharacter.h"
#include "Characters/RedCHeroCharacter.h"
#include "Core/Systems/RedCAssetManager.h"
#include "Core/Systems/RedCItemManager.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GAS/RedCAbilitySet.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "GAS/Attributes/RedCCurrencySet.h"
#include "Types/RedCGameplayTags.h"

class URedCItemManager;


void URedCItemBuyAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
		{
			return;
		}
	}

	UAbilityTask_WaitGameplayEvent* ItemBuyTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SharedTags::Event_Ability_ItemBuy
	);
	ItemBuyTask->EventReceived.AddDynamic(this, &ThisClass::ItemBuyEvent);
	ItemBuyTask->ReadyForActivation();
	AddWatchedTask(ItemBuyTask);
}


void URedCItemBuyAbility::ItemBuyEvent(FGameplayEventData Payload)
{
	const int32 InputNum = Payload.EventMagnitude;

	// 1) 레퍼런스 불러오기
	ARedCHeroCharacter* Hero = Cast<ARedCHeroCharacter>(GetAvatarActorFromActorInfo());
	if (!Hero) return; 

	URedCPlayerAbilitySystemComponent* ASC = Hero->GetRedCPlayerAbilitySystemComponent();
	if (!ASC) return; 

	APlayerStateArena* PS = Cast<APlayerStateArena>(Hero->GetPlayerState());
	if (!PS) return;

	URedCItemManager* ItemManager = Hero->GetWorld()->GetGameInstance()->GetSubsystem<URedCItemManager>();

	// 2) RowName 조회 + 쿨다운태그 제작
	//FName ItemID = ItemManager->FindItemID(InputNum);
	FName ItemID = PS->FindItemID(InputNum);
	FString CoolDownTagString = "Cooldown.Item." + ItemID.ToString();
	FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag(FName(CoolDownTagString),false);

	// 3) Valid Check
	FRedCItemInfo* Item = CheckValidItem(PS, ItemID);							// 아이템 체크
	if (!Item) return;
	if (!CheckValidAbility(ASC, CooldownTag)) return;							// 쿨타임 체크

	// 4) Apply Effect
	const bool bIsHeal = ItemManager->bItemTypeIsHealPotion(Item->ItemID);
	const bool bIsBuff = ItemManager->bItemTypeIsBuffPotion(Item->ItemID);
	
	int32 Price = 0;
	if (bIsHeal || bIsBuff)
	{
		Price = Item->ItemProperty[0].ItemPrice;								// 다음가격 체크(포션)
	}
	else
	{
		Price = CalcNextPrice(*Item, Price);									// 다음가격 체크(아이템)
	}
	if (Price == 0) return;
	if (!CheckValidCoin(ASC, Price)) return;									// 현재코인 체크

	ApplyCoinDecrease(ASC, Price);												// 코인감소
	if (bIsHeal)       { ApplyHealItemStat(ASC, Item); }
	else if (bIsBuff)  { ApplyBuffItemStat(ASC, Item); }
	else               { ApplyItemStat(ASC, Item); }							// 아이템 스탯 적용

	
	ApplyItemCooldown(ASC, CooldownTag, Item->ItemCoolTime);					// 아이템 쿨다운 적용

	// 5) 아이템 레벨업
	if (!bIsHeal && !bIsBuff)
	{
		Item->ItemLevel = FMath::Clamp(Item->ItemLevel + 1, 0, Item->ItemMaxLevel);
		PS->CharacterItemInfo.MarkItemDirty(*Item);
		PS->CharacterLevel += 3;
	}

	K2_EndAbility();
}


# pragma region Valid Check
bool URedCItemBuyAbility::CheckValidAbility(URedCPlayerAbilitySystemComponent* ASC,FGameplayTag CooldownTag)
{
	// 검증 - 쿨타임
	if (ASC->HasMatchingGameplayTag(CooldownTag))
	{
		UE_LOG(LogRedCWidget, Warning, TEXT("[BuyGA] Its Cool Time"));
		K2_EndAbility();
		return false;
	}
	return true;
}

FRedCItemInfo* URedCItemBuyAbility::CheckValidItem(APlayerStateArena* PS, FName ItemID)
{
	// 검증 - RowName
	if (ItemID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuyGA] ItemRowName not found "));
		K2_EndAbility();
		return nullptr;
	}
	
	// 검증 - Item 정보
	FRedCItemInfo* Item = PS->FindItemInfo(ItemID);
	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuyGA] PS has no item: %s"), *ItemID.ToString());
		K2_EndAbility();
		return nullptr;
	}

	return Item;
}

bool URedCItemBuyAbility::CheckValidCoin(URedCPlayerAbilitySystemComponent* ASC, int32 Price)
{
	// 검증 - 코인계산
	const int32 CurrCoin = ASC->GetNumericAttributeBase(URedCCurrencySet::GetCoinAttribute());
	if (Price > CurrCoin)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuyGA] Not enough coin. Need:%d Have:%d"), Price, CurrCoin);
		K2_EndAbility();
		return false;
	}
	return true;
}


int32 URedCItemBuyAbility::CalcNextPrice(const FRedCItemInfo& Item, int32& NextItemPrice)
{
	const int32 Level = Item.ItemLevel;
	if (Level >= Item.ItemMaxLevel)
	{
		UE_LOG(LogTemp, Log, TEXT("[BuyGA] Already max or price not found. L:%d Max:%d"), Item.ItemLevel, Item.ItemMaxLevel);
		K2_EndAbility();
		return 0;
	}
	if (!Item.ItemProperty.IsValidIndex(Level))
	{
		UE_LOG(LogTemp, Log, TEXT("[BuyGA] Already max or price not found. L:%d Max:%d"), Item.ItemLevel, Item.ItemMaxLevel);
		K2_EndAbility();
		return 0;
	}
		
	return Item.ItemProperty[Level].ItemPrice;
}
# pragma endregion

# pragma region ApplyEffect

void URedCItemBuyAbility::ApplyItemCooldown(URedCPlayerAbilitySystemComponent* ASC, FGameplayTag CooldownTag, int32 ItemCooldown)
{
	// 6) 쿨다운
	if (ensureAlways(ItemCooldownGE))
	{
		FGameplayEffectSpecHandle CooldownSpec = ASC->MakeOutgoingSpec(ItemCooldownGE, 1, ASC->MakeEffectContext());

		CooldownSpec.Data->DynamicGrantedTags.AddTag(CooldownTag);
		CooldownSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.Cooldown")), ItemCooldown);
		
		ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpec.Data.Get());
	}
}

void URedCItemBuyAbility::ApplyItemStat_MaxGauge(URedCPlayerAbilitySystemComponent* ASC, float MaxGauge)
{
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle StatSpec = ASC->MakeOutgoingSpec(ItemMaxStatGE, 1.f, ContextHandle);

	const FGameplayTag TagMaxHP = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.MaxHealth"));
	const FGameplayTag TagMaxMP = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.MaxStamina"));
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagMaxHP, MaxGauge);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagMaxMP, 0.f);
	
	ASC->ApplyGameplayEffectSpecToSelf(*StatSpec.Data.Get());
}

void URedCItemBuyAbility::ApplyItemStat(URedCPlayerAbilitySystemComponent* ASC, FRedCItemInfo* Item)
{
	// 5) 스탯 증가
	if (ensureAlways(ItemStatGE))
	{
		const int32 Level = Item->ItemLevel;
		if (Item->ItemProperty.IsValidIndex(Level))
		{
			// Max Gauge 선적용
			if (Item->ItemProperty[Level].MaxHealth > 0
				|| Item->ItemProperty[Level].MaxStamina > 0)
				ApplyItemStat_MaxGauge(ASC, Item->ItemProperty[Level].MaxHealth);
			
			const float AttackPowerToAdd = Item->ItemProperty[Level].AttackPower;
			const float DefenseToAdd = Item->ItemProperty[Level].Defense;
			const float CriticalToAdd = Item->ItemProperty[Level].Critical;
			float HealthToAdd = Item->ItemProperty[Level].Health;
            if (Item->ItemProperty[Level].MaxHealth > 0)
            {
                HealthToAdd += Item->ItemProperty[Level].MaxHealth;
            }
			const float EvasionToAdd = Item->ItemProperty[Level].Evasion;
			const float MovementSpeedToAdd = Item->ItemProperty[Level].MovementSpeed;

			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle StatSpec = ASC->MakeOutgoingSpec(ItemStatGE, 1.f, ContextHandle);
			
			const FGameplayTag TagAtk = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.AttackPower"));
			const FGameplayTag TagDef = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.Defense"));
			const FGameplayTag TagCrt = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.Critical"));
			const FGameplayTag TagHP = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.Health"));
			const FGameplayTag TagEva = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.Evasion"));
			const FGameplayTag TagMov = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.MovementSpeed"));
			
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagAtk, AttackPowerToAdd);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagDef, DefenseToAdd);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagCrt, CriticalToAdd);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagHP, HealthToAdd);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagEva, EvasionToAdd);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagMov, MovementSpeedToAdd);
			
			ASC->ApplyGameplayEffectSpecToSelf(*StatSpec.Data.Get());
		}
	}
}

void URedCItemBuyAbility::ApplyCoinDecrease(URedCPlayerAbilitySystemComponent* ASC, int32 Price)
{
	if (ensureAlways(CoinDeltaGE))
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(CoinDeltaGE, 1.f, ContextHandle);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			Spec,
			FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.Coin")),
			-static_cast<float>(Price)
		);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void URedCItemBuyAbility::ApplyHealItemStat(URedCPlayerAbilitySystemComponent* ASC, FRedCItemInfo* Item)
{
	// 5) 스탯 증가
	if (ensureAlways(GE_ItemHealPotionStat))
	{
		const int32 Level = Item->ItemLevel;
		if (Item->ItemProperty.IsValidIndex(Level))
		{
			float HealthToAdd = Item->ItemProperty[Level].Health;
			
			const float DurationSeconds = 60.f;  
			const float PeriodSeconds   = 1.f;   
			const int32 TickCount = FMath::Max(1, FMath::RoundToInt(DurationSeconds / PeriodSeconds));
			const float HealPerTick = HealthToAdd / TickCount;
			
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle StatSpec = ASC->MakeOutgoingSpec(GE_ItemHealPotionStat, 1.f, ContextHandle);
			
			StatSpec.Data->DynamicGrantedTags.AddTag(
			  FGameplayTag::RequestGameplayTag(TEXT("Effect.Item.Potion")));

			const FGameplayTag TagHP = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.Health"));

			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(StatSpec, TagHP, HealPerTick);
			
			ASC->ApplyGameplayEffectSpecToSelf(*StatSpec.Data.Get());
		}
	}
}



void URedCItemBuyAbility::ApplyBuffItemStat(URedCPlayerAbilitySystemComponent* ASC, FRedCItemInfo* Item)
{
	const int32 Level = Item->ItemLevel;
	if (!Item->ItemProperty.IsValidIndex(Level)) return;

	const float AttackToAdd = Item->ItemProperty[Level].AttackPower;
	const float DefenseToAdd = Item->ItemProperty[Level].Defense;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_ItemBuffPotionStat, 1.f, ContextHandle);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->DynamicGrantedTags.AddTag(
	   FGameplayTag::RequestGameplayTag(TEXT("Effect.Item.Potion"))
   );

	const FGameplayTag TagAtk = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.AttackPower"));
	const FGameplayTag TagDef = FGameplayTag::RequestGameplayTag(TEXT("Data.SetByCaller.Defense"));
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TagAtk, AttackToAdd);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TagDef, DefenseToAdd);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

# pragma endregion


FGameplayTag URedCItemBuyAbility::MakeCoolTimeTagByRowName(FName ItemRowName)
{
	FGameplayTag t;
	return t;
}

