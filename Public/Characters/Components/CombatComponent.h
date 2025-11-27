// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/RedCAbilityTypes.h"
#include "CombatComponent.generated.h"


struct FGameplayTag;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREDC_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma region Cache Actor ASC
private:
	// Getter와 Setter지만 내부에서만 사용하므로 private하게 둬서 무분별하게 외부에서 사용되는 것을 막고자 함.
	void SetActorASCInMap(AActor* Actor) const;
	UAbilitySystemComponent* GetActorASCInMap(AActor* Actor) const;
	UAbilitySystemComponent* TryGetActorASCInMap(AActor* Actor) const;

	// 캐싱된 ASC를 담는 Map
	UPROPERTY()
	mutable TMap<TObjectPtr<AActor>, TObjectPtr<UAbilitySystemComponent>> CachedASCMap;
#pragma endregion
	
#pragma region Hit Reaction Data Processing
public:
	void PushHitMessage(FHitReactMessage& Message);
	FHitReactMessage PopHitMessage();
	void ClearHitMessageContainer();

	FORCEINLINE int32 GetHitReactMessageCount() const {return CachedHitReactMessageCount ;}
	FORCEINLINE bool IsHitReactMessageEmpty() const {return CachedHitReactMessageCount == 0;}
	
private:
	int CachedHitReactMessageCount = 0;
	TQueue<FHitReactMessage> HitReactMessages;
	
	void ProcessHitMessage();
#pragma endregion

#pragma region Apply Damage & Generate Effect data
public:
	// 공격자를 기준으로 나에게 데미지를 주는 함수.
	// Message : Hit 데이터
	// Attacker : 공격자
	UFUNCTION(Server, Reliable)
	void ServerApplyDamageToOwner(const FHitReactMessage& Message, AActor* Attacker);

	// 나를 기준으로 피격자에게 데미지를 주는 함수.
	// Message : Hit 데이터
	// Target : 피격자
	UFUNCTION(Server, Reliable)
	void ServerApplyDamageToTarget(const FHitReactMessage& Message, AActor* Target) const;
private:
	FGameplayEffectSpecHandle GenerateDamageEffectHandle(const FHitReactMessage& Message, AActor* Attacker, AActor* Victim) const;
	FGameplayEffectSpecHandle GenerateEncounterEffectHandle(AActor* Target) const;

#pragma endregion

#pragma region Validation Check
public:
	FORCEINLINE bool IsHitValid(const FHitReactMessage& Message, const AActor* Attacker) const;
private:
	bool IsPlayersHasASC(AActor* Attacker, AActor* Victim) const;

#pragma endregion
	
public:
	void ExecuteGameplayCue(const FHitReactMessage& Message, AActor* Attacker) const;
	void ResetCombatState(AActor* Target);

	void ApplyHitDilationUsingMontage(AActor* DilationActor, float DilationTime) const;
	
};
