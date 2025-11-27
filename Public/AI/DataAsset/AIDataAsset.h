// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "AIDataAsset.generated.h"

enum class EMGAbilityInputID : uint8;

class UGameplayAbility;
class UGameplayEffect;

USTRUCT()
struct FAITeamMaterials
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> BodyMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> LensMaterial = nullptr;
};

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UAIDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//초기 적용 이펙트
	UPROPERTY(EditDefaultsOnly, Category="AI|Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	// AI 사망 시 적용될 이펙트
	UPROPERTY(EditDefaultsOnly, Category="AI|Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;
	
	// AI의 기본 공격/행동 (ex: 평타, 기본 스킬)
	UPROPERTY(EditDefaultsOnly, Category="AI|Abilities")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> BaseAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Effects")
	TSubclassOf<UGameplayEffect> StartStatGE;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Effects")
	TSubclassOf<UGameplayEffect> CurrentStatGE;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Effects")
	FAIData AIData;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Meshes")
	TArray<FAITeamMaterials> TeamMaterials;
};
