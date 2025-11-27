// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_SendActionToASC.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "AbilitySystemComponent.h" 


UBTTask_SendActionToASC::UBTTask_SendActionToASC()
{
	NodeName = "Activate Ability By Tag";
}

EBTNodeResult::Type UBTTask_SendActionToASC::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	ARedCAICharacter* AIChar = Cast<ARedCAICharacter>(AIController->GetPawn());
	if (!AIChar)
	{
		return EBTNodeResult::Failed;
	}
	UAbilitySystemComponent* ASC = AIChar->GetAbilitySystemComponent();
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	if (!AbilityTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	
	if (AIChar->HasAuthority())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(AbilityTag);
	
		bool bActivated = ASC->TryActivateAbilitiesByTag(TagContainer);
		
		return bActivated ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	return EBTNodeResult::Failed;
}