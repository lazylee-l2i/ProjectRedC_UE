// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "RedCPlayerController.generated.h"

class UCrosshairComponent;
class UUIManagerSubsystem;
class UInputConfig;

UENUM(BlueprintType)
enum class EMGInputMode : uint8
{
	None,
	GameOnly,
	GameAndUI,
	UIOnly
};

/**
 * 
 */

UCLASS()
class PROJECTREDC_API ARedCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARedCPlayerController();
	
	virtual void BeginPlay() override;
	virtual void AddPitchInput(float Value) override;
	virtual void AddYawInput(float Value) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	void ClearInputMappingContext();

	UFUNCTION(BlueprintCallable)
	void SetupInputMappingContext();
	
	UFUNCTION(BlueprintPure)
	const UInputConfig* GetInputConfig() const { return InputConfig; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Input", Meta = (Categories = "Input.Context"))
	FGameplayTagContainer ContextTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Input")
	TObjectPtr<UInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Aim", Meta = (ClampMin = 0.1, ClampMax = 1.0))
	float PitchSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = "CustomValues|Aim", Meta = (ClampMin = 0.1, ClampMax = 1.0))
	float YawSensitivity;

	UPROPERTY()
	UUIManagerSubsystem* UIManager;

public:
	/** InputMode 변경 */
	EMGInputMode CurrentInputMode;
	UFUNCTION(BlueprintCallable, Category="Input")
	void SetInputModeEx(EMGInputMode Mode);
	void ChangeGameModeGameAndUI();
	void ChangeGameModeGameOnly();
	void OnAltPressedToggle();
	bool bAltToggle = false;
	

	/** Widget변경 */
	void CreateHudWidget();
	void SetInitMatchingWidget();
	UFUNCTION(Client, Reliable)
	void Client_ShowCharacterSelectionUI();
	void ShowRespawncount();
	void HiddenRespawncount();
	void RemoveLoadingWidget();

	UFUNCTION(Client,Reliable)
	void Client_StartLoadingWidget();

	// 게임 종료 관련 함수
	UFUNCTION(Client, Reliable)
	void Client_ShowGameWinnerUI();
	UFUNCTION(Client, Reliable)
	void Client_ShowGameLoserUI();
	UFUNCTION(Client, Reliable)
	void Client_ShowGameDrawUI();

	/** 캐릭터 선택 정보 */
	// 0 : Warrior
	// 1 : Archor
	UFUNCTION(Server, Reliable)
	void Server_PlayerPickCharacter(int CharacterID);
	
	/** Player MatchQue Register */
	void RegisterPlayerController();
	UFUNCTION(Server, Reliable)
	void Server_RegisterPlayer();

	UPROPERTY(Replicated)
	bool bHasBeenRegistered = false;

	/** Debug */
	UFUNCTION(Server, Reliable)
	void Server_EndGame();
};
