// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_ApplyGE.h"

#include "Types/RedCLogChannels.h"

void UAN_ApplyGE::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (not NotifyValidationCheck(MeshComp))
	{
		return;
	}
	if (IsValid(GameplayEffect))
	{
		if (GameplayEffect.GetDefaultObject()->DurationPolicy == EGameplayEffectDurationType::Infinite)
		{
			UE_LOG(LogRedCEffectManager, Error, TEXT("This Notify Must Use Instant or Duration Policy. Don't Use Infinite Policy."));
			return;
		}
		if (MeshComp->GetOwner()->HasAuthority())
		{
			FGameplayEffectContextHandle ContextHandle = GetMeshAbilitySystemComponent(MeshComp)->MakeEffectContext();
			GetMeshAbilitySystemComponent(MeshComp)->ApplyGameplayEffectToSelf(GameplayEffect.GetDefaultObject(), 0, ContextHandle);
			/*auto Handles = GetMeshAbilitySystemComponent(MeshComp)->GetActiveGameplayEffects().GetAllActiveEffectHandles();
			for (auto& Handle : Handles)
			{
				GetMeshAbilitySystemComponent(MeshComp)->GetActiveGameplayEffect(Handle)->Tag
			}*/
		}
	}
	
}
