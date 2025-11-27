// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/RedCFunctionLibrary.h"

#include "GameplayTagsManager.h"
#include "Types/RedCAbilityTypes.h"


FGameplayTag URedCFunctionLibrary::FindChildTagByName(const FString& Name, const FGameplayTag& ParentTag)
{
	if (not Name.IsEmpty() && ParentTag.IsValid())
	{
		const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

		if (ParentTag.IsValid())
		{
			for (const FGameplayTag& ChildTag : TagsManager.RequestGameplayTagChildren(ParentTag))
			{
				FString FoundName;
				ChildTag.GetTagName().ToString().Split(TEXT("."), nullptr, &FoundName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				if (Name == FoundName)
				{
					return ChildTag;
				}
			}
		}
	}
	
	return FGameplayTag::EmptyTag;
}

FVector URedCFunctionLibrary::GetCustomVector(FGameplayEffectContextHandle EffectContextHandle)
{
	FCustomGameplayEffectContext* EffectContext = static_cast<FCustomGameplayEffectContext*>(EffectContextHandle.Get());

	return EffectContext ? EffectContext->GetCustomVector() : FVector::ZeroVector;
}

EWeaponType URedCFunctionLibrary::GetWeaponType(FGameplayEffectContextHandle EffectContextHandle)
{
	FCustomGameplayEffectContext* EffectContext = static_cast<FCustomGameplayEffectContext*>(EffectContextHandle.Get());

	return EffectContext ? EffectContext->GetWeaponType() : EWeaponType::None;
}

EArmorType URedCFunctionLibrary::GetArmorType(FGameplayEffectContextHandle EffectContextHandle)
{
	FCustomGameplayEffectContext* EffectContext = static_cast<FCustomGameplayEffectContext*>(EffectContextHandle.Get());

	return EffectContext ? EffectContext->GetArmorType() : EArmorType::None;
}

