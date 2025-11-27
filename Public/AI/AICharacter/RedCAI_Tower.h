// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "GAS/Attributes/RedCHealthSet.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Interface/CharacterIdentifier.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "NiagaraSystem.h"

#include "RedCAI_Tower.generated.h"

class USoundCue;
class UWidgetComponent;
class USphereComponent;
class UStaticMeshComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
class UMaterialInterface;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTowerDestroy, AActor*, TowerDestroy);

USTRUCT(BlueprintType)
struct FTowerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = 400.f;
};
/**
 * 
 */
UCLASS()
class PROJECTREDC_API ARedCAI_Tower :	public AActor,
										public IGenericTeamAgentInterface,
										public IAbilitySystemInterface,
										public ICharacterIdentifier
{
	GENERATED_BODY()

public:

	ARedCAI_Tower();
	
	virtual void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return SourceASC;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UAIPerceptionStimuliSourceComponent* StimuliSource = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower|GAS")
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower|GAS")
	URedCHealthSet* HealthSet = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tower|GAS")
	TSubclassOf<UGameplayEffect> GE_TowerInitialize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower|Stats")
	FTowerData TowerData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower")
	USphereComponent* SphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Tower")
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower")
	AActor* CurrentTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="FX")
	UNiagaraSystem* NS_Impact;                 // 포탑 포탄 터지는 나이아가라 지정
	
	void StartAttacking();  // 공격 타이머 시작
	void StopAttacking();   // 공격 타이머 정지
	void AttackTarget();    // 주기적으로 실행할 함수
	
	// 공격 예고 표시
	void ShowAttackIndicator( const FVector& Location); 

	// 모든 클라이언트에 예고표시
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ShowAttackIndicator(FVector Location);
	
	// 타워 내에서 적 죽었을때 타겟 업데이트할때 델리게이트로 받아오기
	UFUNCTION()
	void OnEnemyDied(AActor* DeadActor);
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_TeamId, Category="Tower|Team") 
	FGenericTeamId TeamId = FGenericTeamId::NoTeam;

	UFUNCTION()
	void OnRep_TeamId();

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		return TeamId;
	}

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamId) override;

	virtual FGameplayTag GetCharacterTag() const override;
	virtual uint8 GetCharacterTeamId_UINT8() const override;
	virtual bool IsCharacterSameTeam(const ICharacterIdentifier* Other) const override;

	UFUNCTION(BlueprintCallable, Category="Tower|Team")  
	void SetTeamId(uint8 NewId);
	
	UPROPERTY(BlueprintAssignable, Category="Tower|Event") 
	FOnTowerDestroy OnTowerDestroyed;
	
	UFUNCTION(BlueprintCallable, Category="Tower")
	void TowerDestroy();
	

protected:
	
	void UpdateTarget(); //타겟 변경
	
	//데미지 처리
	void PerformRadialDamage_GAS(FVector Location);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	TArray<AActor*> MinionQueue;
    
	UPROPERTY()
	TArray<AActor*> ChampionQueue;
    
	FTimerHandle AttackTimerHandle;
    
	bool bHasTarget = false;
    	
	UPROPERTY(EditAnywhere, Category="Tower|VFX") // 메쉬
	UStaticMesh* IndicatorMesh = nullptr;
    	
	UPROPERTY(EditAnywhere, Category="Tower|VFX") //머티리얼, 색
	UMaterialInterface* IndicatorMaterial = nullptr;
    	
	UPROPERTY(EditAnywhere, Category="Tower|VFX") //위로 띄우기
	float IndicatorZOffset_Mesh = 2.f;
    	
	UPROPERTY(EditAnywhere, Category="Tower|VFX") //범위
	float IndicatorScale = 2.f;

	UPROPERTY(EditAnywhere, Category="Tower|Attack")
	TArray<TEnumAsByte<EObjectTypeQuery>> AOEObjectTypes;

	UPROPERTY(EditAnywhere, Category="Tower|Attack")
	float DamageRadius = 100.f;					//공격 범위

	UPROPERTY(EditAnywhere, Category="Tower|Attack") 
	float DamageAmount = 150.f;					//데미지
                                                        
	UPROPERTY(EditAnywhere, Category="Tower|Attack") 
	float PreAttackDelay = 2.0f;				// 공격전 보여주는 알림 딜레이
	
    UPROPERTY(EditAnywhere, Category="Tower|Attack")
	float AttackTime = 5.0f;					//공격 주기
		
	UPROPERTY(EditAnywhere, Category="Tower|Attack")
	bool bDrawDebugRadius = false;				//범위 보이는거

	UPROPERTY(EditAnywhere, Category="Tower|FX")
	UNiagaraSystem* NS_ChargeUp;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayChargeFX();

	UPROPERTY(EditDefaultsOnly, Category="Tower|FX")
	UNiagaraSystem* NS_ChargeRelease;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayChargeReleaseFX();
	
	// === GAS 설정 ===
	// 타워가 적용할 데미지 GE (BP_ApplyDamageEffect 등)
	UPROPERTY(EditAnywhere, Category="Tower|GAS")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	// Source ASC를 타워 Owner에서 가져올지 여부(없으면 타워 자신 ASC)
	UPROPERTY(EditAnywhere, Category="Tower|GAS")
	bool bUseOwnerASCAsSource = false;
	
	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	
	static FGenericTeamId ResolveTeamIdFromActor(const AActor* Actor); 
	bool IsHostileTo(const AActor* Other) const;

	UPROPERTY(Transient)    
	bool bIsBeingDestroyed = false;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayFX(const FVector& Location);

	UPROPERTY(EditDefaultsOnly, Category="Tower|FX")
	TMap<FName, USoundCue*> SFX_Map;

	UPROPERTY(EditDefaultsOnly, Category="Tower|FX")
	TMap<FName, UNiagaraSystem*> VFX_Map;

	TWeakObjectPtr<UAbilitySystemComponent> CurrentTargetASC;
	FDelegateHandle TargetDeathTagHandle;

	void BindTargetDeathTag(AActor* NewTarget);
	void UnbindTargetDeathTag();

	UFUNCTION()
	void OnTargetDeathTagChanged(const FGameplayTag Tag, int32 NewCount);
	
private:
	void RegisterTowerToGS();

	void SetTeamColorOverlayMaterial();

	void ConfigureOverheadHealthWidget();
	void UpdateOverheadHealthWidgetVisibility();
	
	bool IsOverheadHealthWidgetOnScreen() const;
	bool IsOverheadHealthWidgetInRange() const;

	FTimerHandle OverheadHealthWidgetUpdateTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|OverheadWidget", Meta = (ClampMin = 0.f))
	float OverheadHealthWidgetCheckInterval;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|OverheadWidget", Meta = (ClampMin = 0.f))
	float OverheadHealthWidgetVisibleRange;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UWidgetComponent> OverheadHealthWidgetComp;

	// 0 : Enemy, 1 : Friend
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|TeamColor")
	TArray<TObjectPtr<UMaterialInterface>> TeamColorOverlayMaterial;
};
