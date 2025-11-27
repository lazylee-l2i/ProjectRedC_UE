// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Interface/CharacterIdentifier.h"
#include "Interface/MotionWarpingInterface.h"
#include "Interface/RedCCombatSystem.h"
#include "Types/RedCAbilityTypes.h"
#include "RedCCharacter.generated.h"

class UWidgetComponent;
class UDefaultMontageData;
class UCombatComponent;
class UBoxComponent;
class UGameplayEffect;
class UMotionWarpingComponent;
class UContextualAnimSceneActorComponent;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterDead, int32);


UCLASS()
class PROJECTREDC_API ARedCCharacter :	public ACharacter,
										public IAbilitySystemInterface,
										public IMotionWarpingInterface,
										public ICharacterIdentifier,
										public IRedCCombatSystem
{
	GENERATED_BODY()

#pragma region Initialization
public:
	ARedCCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void InitAbilitySystemComponent();
	virtual void BindCallbacksToDependencies();
	
	bool IsCharacterHasTag(const FGameplayTag& Tag) const;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	FORCEINLINE USkeletalMeshComponent* GetSkeletalMeshComponent() const { return GetMesh();}

protected:
	virtual void ResolveCharacterDefinition();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CharacterTag;
#pragma endregion
	
#pragma region Hit Box Collision
protected:
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* StandingHitBox;

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* KnockdownHitBox;

public:
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	virtual void SwitchHitBoxStateToStanding();
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	virtual void SwitchHitBoxStateToKnockdown();

#pragma endregion

#pragma region Motion Warping
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComp;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UContextualAnimSceneActorComponent> ContextualAnimSceneActorComp;
public:
	UContextualAnimSceneActorComponent* GetContextualAnimSceneActorComponent() const;
	
	UFUNCTION(Server, Reliable)
	void ServerRunMotionWarping(class UContextualAnimSceneAsset* Asset, AActor* TargetActor, UAnimMontage* Montage);

	UFUNCTION(Server, Reliable)
	void ServerStopAllMontage(float BlendOutDuration = 0.f);
	
	UFUNCTION(NetMulticast, Reliable)
	void PrepareCharacterNextAnim(float BlendOutDuration = 0.f);
	
	virtual UMotionWarpingComponent* GetMotionWarpingComponent() const override;
	virtual void SetMotionWarpingTarget(FMotionWarpingParam& MotionWarpingParam) override;
	virtual void RemoveAllMotionWarpingTarget() override;
	virtual uint8 GetCurrentMovementEffect() override { return 0;} //자식에서 구현하고 필요없으면 Super 호출 하지마셈.
#pragma endregion

#pragma region Rotation Locking & Force Setting Rotation
public:
	UFUNCTION(NetMulticast, Reliable)
	void UnlockControllerRotation();

	UFUNCTION(NetMulticast, Reliable)
	void LockControllerRotation();

	UFUNCTION(NetMulticast, Reliable)
	void ForceSetCharacterRotation(FRotator NewRotation);
#pragma endregion

#pragma region Get-Up(Recovery Battle State)
public:
	void StartGetUpTimer();
	
	void CancelGetUpTimer();

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> GetUpMontage;
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayMontage(UAnimMontage* Montage);

private:
	UPROPERTY()
	FTimerHandle GetUpTimerHandle;

	UFUNCTION()
	void GetUpTimerCallback();
	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = true))
	float GetUpTimerDuration = 3.5f;

	UPROPERTY()
	float GetUpStartTime = 0.0f;
	
	float RemainTime = 0.0f;
#pragma endregion

#pragma region Hit React
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCombatComponent> CombatSystem;
	
public:
	FORCEINLINE virtual UCombatComponent* GetCombatComponent() const override {return CombatSystem;}

	UFUNCTION(Server, Reliable)
	// 이름 때문에 오해할 수 있는데, 이거 피격자한테 보내는게 아니라 RPC 오너쉽 때문에
	// 우선, 타격자 Character한테 보내는거임.
	void ServerReceiveHitReactData(FHitReactMessage Message, AActor* HitActor);
	
	UFUNCTION(Client, Reliable)
	void ClientActivateHitReact(const FGameplayTag& HitStrength, FHitReactMessage Message);

	void ProcessHitReaction(const FGameplayTag& HitStrength);

#pragma endregion
	
#pragma region Death & Respawn
protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> RespawnMontage;
	
	UPROPERTY()
	TObjectPtr<UDefaultMontageData> MontageData;

	virtual void OnDeath() {}
	virtual void OnRespawn() {}
	
public:
	FOnCharacterDead OnCharacterDead;

private:
	void DeathTagUpdated(FGameplayTag GameplayTag, int32 Count);
	void RemovePotionEffects_OnDeath();
	void StartDeathSequence();
	void Respawn();
	
	// void EnterInvincibleState(FGameplayTag GameplayTag, int Count) const;

	UPROPERTY()
	mutable FActiveGameplayEffectHandle CachedInvincibleEffectHandle;
	
	UFUNCTION()
	void OnRep_Dead();

	UPROPERTY(ReplicatedUsing = OnRep_Dead)
	bool IsDead;

protected:
	UFUNCTION(Client, Reliable)
	void ClientRespawn(FRotator Rotation);

#pragma endregion

#pragma region Team ID
public:
	virtual uint8 GetCharacterTeamId_UINT8() const override;
	virtual bool IsCharacterSameTeam(const ICharacterIdentifier* Other) const override;

protected:
	void UpdateCachedTeamId(FGenericTeamId NewTeamId);

	UFUNCTION()
	virtual void OnRep_CachedTeamId();

private:
	UPROPERTY(ReplicatedUsing = OnRep_CachedTeamId)
	FGenericTeamId CachedTeamId;
#pragma endregion

#pragma region Overhead Health Widget
protected:
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
#pragma endregion

public:
	// Utility
	FString GetActorLocalRoleName() const;

	// Character Info
	UFUNCTION(BlueprintPure)
	FString GetMyTag() const { return CharacterTag.ToString(); }

	virtual FGameplayTag GetCharacterTag() const override;
};
