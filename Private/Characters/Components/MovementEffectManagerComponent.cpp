// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/MovementEffectManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/RedCHeroCharacter.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"

#pragma region Initilization
class ARedCHeroCharacter;

UMovementEffectManagerComponent::UMovementEffectManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bReplicateUsingRegisteredSubObjectList = true;
	
}

void UMovementEffectManagerComponent::RegisterOwnerASC(UAbilitySystemComponent* ASC)
{
	if (IsValid(ASC))
	{
		OwnerASC = ASC;
	}
	else
	{
		UE_LOG(LogRedCEffectManager, Error, TEXT("Might be ASC is not ready to register. check life cycle. %s"), *GetNameSafe(this));
	}
}
#pragma endregion

#pragma region Movement Effect Methods
void UMovementEffectManagerComponent::ApplyMovementEffect(const FVector2D& Input2D)
{
	if (not IsValid(OwnerASC))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
		{
			OwnerASC = ASC;
		}
		//return;
	}
	
	
	// Effect는 아래 함수에서 진행됨.
	if (not OwnerASC->HasMatchingGameplayTag(PlayerAbilityTags::Player_Ability_Movement_Sprint))
	{
		const ARedCHeroCharacter* PlayerCharacter = GetOwner<ARedCHeroCharacter>();

		if (Input2D.Y >= 0.0f)
		{
			if (not OwnerASC->HasMatchingGameplayTag(PlayerEffectTags::Player_Effect_Speed_Forward))
			{
				PlayerCharacter->ServerApplyGameplayEffect(PlayerEffectTags::Player_Effect_Speed_Forward);	
			}
		}
		else if (Input2D.Y < 0.0f)
		{
			if (not OwnerASC->HasMatchingGameplayTag(PlayerEffectTags::Player_Effect_Speed_Backward))
			{
				PlayerCharacter->ServerApplyGameplayEffect(PlayerEffectTags::Player_Effect_Speed_Backward);
			}
		}
	}
}

void UMovementEffectManagerComponent::ApplyMovementGameplayEffectByTag(const FGameplayTag EffectTag)
{
	if (IsValid(OwnerASC))
	{
		if (MovementEffectClasses.Contains(EffectTag) && not MovementEffectHandles.Contains(EffectTag))
		{
			if (EffectTag.MatchesTag(PlayerEffectTags::Player_Effect_Speed))
			{
				EraseMovementEffect();
			}
			const FActiveGameplayEffectHandle Handle = OwnerASC->ApplyGameplayEffectToSelf(
			MovementEffectClasses[EffectTag].GetDefaultObject(), 0, OwnerASC->MakeEffectContext());
			MovementEffectHandles.Add(EffectTag, Handle);
		}
	}
}

void UMovementEffectManagerComponent::RemoveMovementEffectByTag(const FGameplayTag EffectTag)
{
	if (IsValid(OwnerASC))
	{
		if (MovementEffectHandles.Contains(EffectTag))
		{
			OwnerASC->RemoveActiveGameplayEffect(MovementEffectHandles[EffectTag]);
			MovementEffectHandles.Remove(EffectTag);
		}
	}
	else
	{
		UE_LOG(LogRedCEffectManager, Warning, TEXT("OwnerASC is not registered. %s"), *GetNameSafe(this));
	}
}

void UMovementEffectManagerComponent::EraseMovementEffect()
{
	if (IsValid(OwnerASC))
	{
		if (MovementEffectHandles.Find(PlayerEffectTags::Player_Effect_Speed_Forward))
		{
			OwnerASC->RemoveActiveGameplayEffect(MovementEffectHandles[PlayerEffectTags::Player_Effect_Speed_Forward]);
			MovementEffectHandles.Remove(PlayerEffectTags::Player_Effect_Speed_Forward);
		}
		if (MovementEffectHandles.Find(PlayerEffectTags::Player_Effect_Speed_Backward))
		{
			OwnerASC->RemoveActiveGameplayEffect(MovementEffectHandles[PlayerEffectTags::Player_Effect_Speed_Backward]);
			MovementEffectHandles.Remove(PlayerEffectTags::Player_Effect_Speed_Backward);
		}
		if (MovementEffectHandles.Find(PlayerEffectTags::Player_Effect_Speed_Sprint))
		{
			OwnerASC->RemoveActiveGameplayEffect(MovementEffectHandles[PlayerEffectTags::Player_Effect_Speed_Sprint]);
			MovementEffectHandles.Remove(PlayerEffectTags::Player_Effect_Speed_Sprint);
		}
	}
}

#pragma endregion