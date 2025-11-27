// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Statics/RedCAbilitySystemStatics.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Types/RedCGameplayTags.h"

void URedCAbilitySystemStatics::ExecuteGameplayCue(const FGameplayTag& GameplayCueTag, AActor* Target)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
	{
		
	}
}
