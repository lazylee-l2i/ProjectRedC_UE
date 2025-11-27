// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OneKeySkillBox.h"
#include "TwoKeySkillBox.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "UI/MVVM/BattleViewModel.h"
#include "UI/Widget/_Base/Component/BaseSkillBox.h"
#include "SkillBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API USkillBoxWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetBattleViewModelBinding(UBattleViewModel* ViewModel);

	void OnStartedSkillCoolTime(ECharacterAttackType SkillName, int CooldownTime);
public:
	UPROPERTY(meta=(BindWidget))
	UWrapBox* Left_SkillBoxes;

	UPROPERTY(meta=(BindWidget))
	UOneKeySkillBox* SlotBox_E;

	UPROPERTY(meta=(BindWidget))
	UOneKeySkillBox* SlotBox_F;
	
	UPROPERTY(meta=(BindWidget))
	UOneKeySkillBox* SlotBox_Space;

	
	UPROPERTY(meta=(BindWidget))
	UWrapBox* Right_SkillBoxes;

	UPROPERTY(meta=(BindWidget))
	UTwoKeySkillBox* SlotBox_ShiftRightMouse;

	UPROPERTY(meta=(BindWidget))
	UTwoKeySkillBox* SlotBox_ShiftLeftMouse;

	UPROPERTY(meta=(BindWidget))
	UOneKeySkillBox* SlotBox_RightMouse;
	
};
