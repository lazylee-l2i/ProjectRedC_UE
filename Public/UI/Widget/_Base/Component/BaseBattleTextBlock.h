// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "BaseBattleTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBaseBattleTextBlock : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* BaseText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Content", meta=(ExposeOnSpawn=true))
	FText InitialText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	int32 FontSize = 24;

};
