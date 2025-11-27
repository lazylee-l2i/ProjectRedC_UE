// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_ComboEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "Types/RedCLogChannels.h"


void UAN_ComboEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (not NotifyValidationCheck(MeshComp))
	{
		return;
	}
	//UE_LOG(LogRedC, Log, TEXT("Combo Event : %s"), *GetNotifyName_Implementation());
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
	
}

FString UAN_ComboEvent::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);

		if (TagNames.Num() > 0)
		{
			return TagNames.Last().ToString();	
		}
	}
	
	return "None";
}
