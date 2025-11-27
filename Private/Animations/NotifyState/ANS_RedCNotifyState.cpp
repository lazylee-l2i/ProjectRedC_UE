// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/NotifyState/ANS_RedCNotifyState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Interface/CharacterIdentifier.h"
#include "Types/RedCLogChannels.h"


bool UANS_RedCNotifyState::NotifyValidationCheck(const USkeletalMeshComponent* MeshComp)
{
	if (not MeshComp)
	{
		return false;
	}

	if (not MeshComp->GetOwner())
	{
		return false;
	}

	if (not UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		return false;
	}
	return true;
}

bool UANS_RedCNotifyState::CheckSameTeam(const USkeletalMeshComponent* MeshComp, AActor* Other) const
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
