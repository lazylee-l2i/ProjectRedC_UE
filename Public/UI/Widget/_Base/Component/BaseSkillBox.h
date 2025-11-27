// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "BaseSkillBox.generated.h"

class UBaseBattleTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBaseSkillBox : public UUserWidget
{
	GENERATED_BODY()

	
public:
	FWidgetAnimationDynamicEvent EndDelegate;
	
	// CoolDown
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* SkillCoolDownAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* CoolDownFinished;
	
	void StartCoolTime(int32 CooldownTime);
	UFUNCTION()
	void UpdateCooldownText();
	UFUNCTION()
	void EndSkillCoolDown();

	FTimerHandle CooldownTimerHandle;
	float RemainingSeconds = 0;
	
	UPROPERTY(meta=(BindWidget))
	UImage* Img_Cooldown;
	
	UPROPERTY(meta=(BindWidget))
	UImage* Img_GrayBG;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SkillCoolDownText;

	// Property
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* Box_Total;

	UPROPERTY(meta=(BindWidget))
	UOverlay* Overlay_Slot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FSlateBrush IconBrush;
	UPROPERTY(meta=(BindWidget))
	UImage* Img_Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWidgetTransform IconRenderTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FVector2D IconSize = FVector2D(64.f, 64.f);
	
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_SkillName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Text_SkillFont;

	
};
