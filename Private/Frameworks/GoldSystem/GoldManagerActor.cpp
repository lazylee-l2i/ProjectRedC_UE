// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GoldSystem/GoldManagerActor.h"

#include "GameplayEffectTypes.h"
#include "Characters/RedCHeroCharacter.h"
#include "Frameworks/RedCPlayerStateBase.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "Types/RedCLogChannels.h"
#include "Utility/LogHelper.h"


// Sets default values
AGoldManagerActor::AGoldManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OnPersonalGold.AddDynamic(this, &AGoldManagerActor::GivePersonalGold);
	OnGlobalGold.AddDynamic(this, &AGoldManagerActor::GiveGlobalGold);
	OnSpentGold.AddDynamic(this, &AGoldManagerActor::SpentGold);
}

// 초당 골드 지급
void AGoldManagerActor::GiveGlobalGold_Implementation(AActor* Character)
{
	ARedCHeroCharacter* Player = Cast<ARedCHeroCharacter>(Character);
	if (not Player) return; 

	URedCPlayerAbilitySystemComponent* AbilitySystemComponent = Player->GetRedCPlayerAbilitySystemComponent();
	if (not AbilitySystemComponent) return;
	
	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(GlobalEffect, 1, Context);

	FGameplayTag GoldTag = FGameplayTag::RequestGameplayTag(FName("Data.SetByCaller.Coin"));
	Spec.Data->SetSetByCallerMagnitude(GoldTag, 30);

	/*
	 *		모든 유저에게 Global Gold Effect 실행 후, 반환결과물인 EffectHandle을
	 *		멤버변수 TArray에 보관.
	 *
	 *		추후, Global Gold의 값이 달라 질 경우 EffectHandle을 통해
	 *		동작중인 Effect를 전부 지우고, 새로운 Effect로 변경
	 */
	
	
	GlobalGoldEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

}

// 액티비티 골드 지급
void AGoldManagerActor::GivePersonalGold_Implementation(AActor* Character, EGoldRewardType RewardType)
{
	ARedCHeroCharacter* Player = Cast<ARedCHeroCharacter>(Character);
	if (not Player) return;

	URedCPlayerAbilitySystemComponent* AbilitySystemComponent = Player->GetRedCPlayerAbilitySystemComponent();
	if (not AbilitySystemComponent) return;

	ARedCPlayerStateBase* PlayerState = Cast<ARedCPlayerStateBase>(Player->GetPlayerState());
	if (not PlayerState) return;

	// Attribute 값은 float.
	const float Gold = GetGoldByType(RewardType);
	
	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(PersonalEffect, 1, Context);

	FGameplayTag GoldTag = FGameplayTag::RequestGameplayTag(FName("Data.SetByCaller.Coin"));
	Spec.Data->SetSetByCallerMagnitude(GoldTag, Gold);
	
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

	//LogHelper::Print(LogGoldManagerActor, ELogVerbosity::Warning, "PersonalGold Effect Execution", this, false);
}

// 골드 소모(상점 구매)
void AGoldManagerActor::SpentGold_Implementation(AActor* Character, EGoldItemType ItemType)
{
	ARedCHeroCharacter* Player = Cast<ARedCHeroCharacter>(Character);
	if (not Player) return; 

	URedCPlayerAbilitySystemComponent* AbilitySystemComponent = Player->GetRedCPlayerAbilitySystemComponent();
	if (not AbilitySystemComponent) return;

	// Attribute 값은 float.
	const float Gold = SpentGoldByType(ItemType);
	
	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(PersonalEffect, 1, Context);

	FGameplayTag GoldTag = FGameplayTag::RequestGameplayTag(FName("Data.SetByCaller.Coin"));
	Spec.Data->SetSetByCallerMagnitude(GoldTag, Gold);
	
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

	//LogHelper::Print(LogGoldManagerActor, ELogVerbosity::Warning, "SpentGold Effect Execution", this, false);
}

// 타입별 골드 매핑
#pragma region GoldMapping
float AGoldManagerActor::GetGoldByType(EGoldRewardType Type)
{
	switch (Type)
	{
	case EGoldRewardType::None:		return 0;
	case EGoldRewardType::Minion:   return 100;
	case EGoldRewardType::Champion: return 200;
	case EGoldRewardType::Tower:    return 300;
	default: return 0;
	}
}

float AGoldManagerActor::SpentGoldByType(EGoldItemType Type)
{
	switch (Type)
	{
		case EGoldItemType::None:	return 0;
		case EGoldItemType::Glove:	return -550;
		case EGoldItemType::Head:	return -650;
		case EGoldItemType::Chest:	return -800;
		case EGoldItemType::Belt:	return -650;
		case EGoldItemType::Pants:	return -650;
		case EGoldItemType::Boots:	return -650;
		case EGoldItemType::Potion:	return -100;
	default: return 0;
	}
}
#pragma endregion

// Called when the game starts or when spawned
void AGoldManagerActor::BeginPlay()
{
	Super::BeginPlay();
	
}
