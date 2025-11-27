//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RedCCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GenericTeamAgentInterface.h"
#include "AI/GAS/RedCAIAbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"
#include "Animation/AnimMontage.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimInstance.h" 
#include "Types/RedCGameplayTags.h"
#include "RedCAICharacter.generated.h"


class ARedC_AISpawnPoint;
class UAIDataAsset;
class ARedCAISpawnActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIDeath, ARedCAICharacter*, DeadAI);

USTRUCT(BlueprintType)
struct FAIData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackPower = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Defense = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentHealth = 400.f;  
};

/**
 * 
 */

UCLASS()
class PROJECTREDC_API ARedCAICharacter : public ARedCCharacter, public IGenericTeamAgentInterface    
{
	GENERATED_BODY()

public:
	ARedCAICharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ActivateAI();
	virtual void DeactivateAI();

	virtual void SpawnDefaultController() override; // 공백으로 덮어버리기
	
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	
	UFUNCTION(BlueprintCallable, Category="AI")
	void OnDead();
	void RequestReturnToPool();

	UPROPERTY(BlueprintAssignable, Category="AI")
	FOnAIDeath OnAIDeath;

	UPROPERTY()
	class URedCCombatSet* AIComAttrSet = nullptr;

	UPROPERTY()
	class URedCHealthSet* AIHpAttrSet = nullptr;

	// 시야 감지 등록 
	void RegisterStimuli();

	//시야 감지 해제 
	void UnRegisterStimuli();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TWeakObjectPtr<class ARedCAISpawnActor> OwnerSpawner; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	int32 GenerationId = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UAIDataAsset* AIConfig;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsActiveInWorld = false;
	
	UPROPERTY()
	bool bPerceptionDelegatesBound = false;

	FTimerHandle StimuliTimerHandle;
	
	virtual void Revive(const TArray<ARedC_AISpawnPoint*>& InWaypoints);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State")
	bool bReturn = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsDead = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UAIPerceptionStimuliSourceComponent* StimuliSource;
    
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const; 
    	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities")
	URedCAIAbilitySystemComponent* AbilitySystemComponent;

	//ASC 초기화
	void InitializeAbilitiesAndEffects();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveTo")
	TArray<ARedC_AISpawnPoint*> Waypoints;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MoveTo")
	int32 CurrentWaypointIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveTo")
	float MoveSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TWeakObjectPtr<class ARedCAISpawnActor> OwnerSpawnerWeak = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities")
	bool bAbilitiesInitialized = false;  // 능력은 1회만 부여

	UPROPERTY(VisibleInstanceOnly)
	bool bFirstSpawn = true;
	
	//////////////////////////////////////
	///
	// 자동 사망 테스트용 타이머 핸들
	FTimerHandle AutoDeathTimerHandle;

	// 테스트용 자동 사망 시간 (디폴트: 5초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Test")
	float AutoDeathDelay = 5.0f;

	virtual void OnRep_Controller() override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="AI")
	FGenericTeamId TeamID = FGenericTeamId::NoTeam;
	
	// IGenericTeamAgentInterface 구현
	virtual  FGenericTeamId GetGenericTeamId() const override
	{
		return TeamID;
	}

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override
	{
		TeamID = InTeamID;
	}

	void SetCharacterStopStateInBlackBoard(bool bStop);

protected:
	UPROPERTY(EditDefaultsOnly, Category="AI|Tags")
	FGameplayTagContainer StopMoveTagList;
	
	virtual void InitAbilityActorInfo();
	UPROPERTY()
	AController* CachedController; // 풀 재활용용 컨트롤러 캐시
	
	void ApplyAIStatsFromConfig(bool bApplyStartStats, bool bApplyCurrentHealth);

	void EnsureASCActorInfo();

	FTimerHandle DeathTimerHandle;

	FDelegateHandle KnockdownTagDelegateHandle;

	static const FName BBKey_StopMoveByTag;
	
	virtual void OnDeath() override;
	
	bool ShouldStopMove() const;
	
	UFUNCTION()
	void OnMontageStarted(UAnimMontage* Montage);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnStopMoveTagChanged(const FGameplayTag InTag, int32 NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Animation")
	FName BlockMoveMontageName = FName(TEXT("AM_Minion_HeavyHitReact"));

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Animation")
	bool bIsMontagePlaying = false;

private:
	void SetTeamColorMaterial();
};
