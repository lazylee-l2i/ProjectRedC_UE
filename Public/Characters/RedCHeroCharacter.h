// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Characters/RedCPlayerCharacter.h"
#include "Interface/RedCAbilitySystemInterface.h"
#include "RedCHeroCharacter.generated.h"

class URedCAbilitySet;
class URedCProjectileSet;
class UCrosshairComponent;
class UMovementEffectManagerComponent;
class URedCPlayerAbilitySystemComponent;
class UCharacterDefinition;

struct FOnAttributeChangeData;
struct FHitReactData;
struct FCustomTargetData;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API ARedCHeroCharacter : public ARedCPlayerCharacter, public IRedCAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region Initialization
public:
	ARedCHeroCharacter();

	UFUNCTION()
	void ResetActorPosition();

	UFUNCTION()
	void PlayerStateTagsUpdated(const FGameplayTag GameplayTag, const int32 Count);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	FORCEINLINE UCharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }

	// First Init Ability & Attribute (Server-Only)
	UFUNCTION(Server, Reliable)
	void ApplyStartupAbilitiesAndAttributes();

protected:
	virtual void ResolveCharacterDefinition() override;
	
private:
	void SetupCharacterMeshComponent();
	virtual void InitAbilitySystemComponent() override;
	virtual void BindCallbacksToDependencies() override;
	void ApplyRespawnEffect();
	void InitGenericTeamId();
	void InitIdentificationComponent();
	void ApplyTeamCollision();
	void SetMeshOverlayMaterial(bool Enable);

	UPROPERTY(Transient)
	TObjectPtr<UCharacterDefinition> CharacterDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomValues|Material", Meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> OverlayMaterial;

#pragma endregion

#pragma region Component
public:
	URedCPlayerAbilitySystemComponent* GetRedCPlayerAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UCrosshairComponent* GetCrosshairComponent_Implementation() const override;

private:
	UPROPERTY(Transient)
	TObjectPtr<URedCPlayerAbilitySystemComponent> AbilitySystemComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<UCrosshairComponent> CrosshairComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> ProjectileSpawnComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> IdentificationComp;
	
#pragma endregion

public:
	virtual void OnRep_CachedTeamId() override;
	
#pragma region Movement Handling
private:
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<UMovementEffectManagerComponent> MovementEffectManagerComp;

	float CachedMaxStamina;
	
public:
	UFUNCTION(Server, Reliable)
	void ServerApplyGameplayEffect(FGameplayTag EffectTag) const;
	
	UFUNCTION(Server, Reliable)
	void ServerRemoveGameplayEffect(FGameplayTag EffectTag) const;

	UFUNCTION(Server, Reliable)
	void ServerRemoveMovementEffect() const;

	// User Input Cache
	UFUNCTION(Server, Reliable)
	void SetForwardInput(bool bIsPressingForward);
	UFUNCTION(Server, Reliable)
	void SetBackwardInput(bool bIsPressingBackward);

	FORCEINLINE bool IsPressingForward() const { return bForwardPressing;}
	FORCEINLINE bool IsPressingBackward() const{ return bBackwardPressing;}

	virtual uint8 GetCurrentMovementEffect() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void OnSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnStaminaChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	UPROPERTY(Replicated)
	bool bForwardPressing = false;

	UPROPERTY(Replicated)
	bool bBackwardPressing = false;
	
#pragma endregion
	
#pragma region Player State Handle
protected:
	virtual void OnDeath() override;
	virtual void OnRespawn() override;

	virtual void SwitchHitBoxStateToStanding_Implementation() override;

public:
	bool IsInvincible() const;
#pragma endregion Player State Handle

#pragma region Overhead Info Widget
protected:
	void ConfigureOverheadInfoWidget();
	void UpdateOverheadInfoWidgetRotation();
	void UpdateOverheadInfoWidgetVisibility(bool bShouldHide);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> OverheadInfoWidgetComp;
#pragma endregion
};
