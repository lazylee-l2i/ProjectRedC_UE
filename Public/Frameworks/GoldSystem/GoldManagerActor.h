// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "GoldManagerActor.generated.h"

// 타입 정의
UENUM(BlueprintType)
enum class EGoldRewardType : uint8
{
	None,
	Minion,
	Champion,
	Tower
};

UENUM(BlueprintType)
enum class EGoldItemType : uint8
{
	None,
	Glove,
	Head,
	Chest,
	Belt,
	Pants,
	Boots,
	Potion
};
/*
 *		
 *
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGiveGlobalGoldDele, AActor*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGivePersonalGoldDele, AActor*, KillerActor, EGoldRewardType, RewardType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUseGoldDele, AActor*, UsedActor, EGoldItemType, ItemType);

UCLASS()
class PROJECTREDC_API AGoldManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoldManagerActor();

	// Delegate 인스턴스
	FOnGiveGlobalGoldDele OnGlobalGold;
	FOnGivePersonalGoldDele OnPersonalGold;
	FOnUseGoldDele OnSpentGold;
	
	// Delegate 연동 함수
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void GiveGlobalGold(AActor* Character);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void GivePersonalGold(AActor* Character, EGoldRewardType RewardType);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SpentGold(AActor* Character, EGoldItemType ItemType);

	// Enum Type 매핑 함수
	UFUNCTION()
	float GetGoldByType(EGoldRewardType Type); // 골드 지급
	UFUNCTION()
	float SpentGoldByType(EGoldItemType Type); // 골드 사용

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Gameplay Effects
	UPROPERTY(EditAnywhere, Category="Gold Effect")
	TSubclassOf<class UGameplayEffect> PersonalEffect;

	UPROPERTY(EditAnywhere, Category="Gold Effect")
	TSubclassOf<class UGameplayEffect> GlobalEffect;

	// 모든 플레이어의 Global Gold Effect Handle을 담는 TArray를 만들어서 Effect 실행 시
	// 여기 TArray에 보관
private:
	UPROPERTY()
	FActiveGameplayEffectHandle GlobalGoldEffectHandle;
	
#pragma endregion
};
