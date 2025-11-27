// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AbilitySystemComponent.h"


#include "BTTask_SendActionToASC.generated.h"
class URedCAbilitySystemComponent;

UENUM(BlueprintType)
enum class EMGAbilityInputID : uint8
{
	None		UMETA(DisplayName="None"),
	Ranged		UMETA(DisplayName="Ranged"),
	Melee		UMETA(DisplayName="Melee"),
	Tower		UMETA(DisplayName="Tower"),
};
/**
 * 
 */
UCLASS()
class PROJECTREDC_API UBTTask_SendActionToASC : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SendActionToASC();
	
	UPROPERTY(EditAnywhere, Category="Ability")
	FGameplayTag AbilityTag;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
