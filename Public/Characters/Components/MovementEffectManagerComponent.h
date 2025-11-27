// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "MovementEffectManagerComponent.generated.h"


class UGameplayEffect;
class URedCPlayerAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREDC_API UMovementEffectManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	/**********************************************
	 
	MovementEffectManagerComponent는 Server에서 동작이 이뤄짐.
	
	 **********************************************/

#pragma region Initilization
public:
	UMovementEffectManagerComponent();
	void RegisterOwnerASC(UAbilitySystemComponent* ASC);
private:
	TMap<FGameplayTag, FActiveGameplayEffectHandle> MovementEffectHandles;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Movement Effects")
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> MovementEffectClasses;	
	
#pragma endregion

#pragma region Effect Control Methods
public:
	void ApplyMovementGameplayEffectByTag(FGameplayTag EffectTag);
	
	void RemoveMovementEffectByTag(FGameplayTag EffectTag);
	
	void EraseMovementEffect();

	// 입력 방향에 따른 처리와 Sprint 시 Effect 예외 처리를 위해 분리한 함수
	void ApplyMovementEffect(const FVector2D& Input2D);
#pragma endregion
};
