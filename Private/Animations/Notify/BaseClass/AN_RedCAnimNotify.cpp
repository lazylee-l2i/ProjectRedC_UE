// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/BaseClass/AN_RedCAnimNotify.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interface/CharacterIdentifier.h"

bool UAN_RedCAnimNotify::NotifyValidationCheck(const USkeletalMeshComponent* MeshComp) const
{
	if (not MeshComp)
	{
		return false;
	}
	
	if (not MeshComp->GetOwner() || not UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		return false;
	}
	return true;
}

bool UAN_RedCAnimNotify::CheckSameTeam(const USkeletalMeshComponent* MeshComp, AActor* Other) const
{
	if (ICharacterIdentifier* CharacterIdentifier = Cast<ICharacterIdentifier>(MeshComp->GetOwner()))
	{
		if (ICharacterIdentifier* OtherCharacterIdentifier = Cast<ICharacterIdentifier>(Other))
		{
			return CharacterIdentifier->IsCharacterSameTeam(OtherCharacterIdentifier);
		}
	}
	return false;
}

UAbilitySystemComponent* UAN_RedCAnimNotify::GetMeshAbilitySystemComponent(const USkeletalMeshComponent* MeshComp) const
{
	if (NotifyValidationCheck(MeshComp))
	{
		return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());	
	}
	return nullptr;
}
