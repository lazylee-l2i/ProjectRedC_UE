// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/RedCAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "Characters/RedCHeroCharacter.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GAS/RedCAbilitySet.h"
#include "GAS/Abilities/RedCGameplayAbility.h"
#include "GAS/Attributes/RedCAttributeSet.h"
#include "GAS/Attributes/RedCHealthSet.h"
#include "GAS/Effects/DeathEffect.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"


URedCAbilitySystemComponent::URedCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(URedCHealthSet::GetHealthAttribute()).AddUObject(this, &URedCAbilitySystemComponent::OnHealthChanged);	
}

void URedCAbilitySystemComponent::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (not GetOwner()) return;

	//if (Cast<ARedCAICharacter>(GetAvatarActor())) return;	// AI Character ( AICharacter.cpp

	if (not GetOwner()->HasAuthority()) return;
	
	if (OnAttributeChangeData.NewValue <= 0)
	{
		if (ARedCCharacter* Character = Cast<ARedCCharacter>(GetAvatarActor()))
		{
			if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character)->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
			{
				return;
			}
			// To-Do Player Respawn Code
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(UDeathEffect::StaticClass(), 1.f, MakeEffectContext());

			if (EffectSpecHandle.IsValid())
			{
				float RespawnTimer = 0.f;
				// 나중에 Level 만들어지면, 아래 코드 수정해야함. 리스폰 시간을 Level * 0.9초로 설정
				if (ARedCHeroCharacter* Player = Cast<ARedCHeroCharacter>(Character))
				{
					RespawnTimer = Player->GetPlayerState<APlayerStateArena>()->CharacterLevel * 0.8f + 7.f;	
				}
				else
				{
					RespawnTimer = 60.f;
				}
				EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_RespawnTimer, RespawnTimer);
				ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
			}
		}
		else
		{
			UE_LOG(LogRedC, Warning, TEXT("Unknown Character Type"));
		}
		
	}
}

void URedCAbilitySystemComponent::GrantGameplayAbilities(const TArray<FRedCAbilityInfo>& Abilities)
{
	for (const FRedCAbilityInfo& Ability : Abilities)
	{
		if (Ability.AbilityClass)
		{
			FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability.AbilityClass, Ability.AbilityLevel);

			if (const URedCGameplayAbility* GameplayAbility = Cast<URedCGameplayAbility>(Spec.Ability))
			{
				if (GameplayAbility->InputTag.IsValid())
				{
					Spec.DynamicAbilityTags.AddTag(GameplayAbility->InputTag);
				}
			}

			GiveAbility(Spec);
		}
	}
}

void URedCAbilitySystemComponent::ApplyGameplayEffects(const TArray<FRedCEffectInfo>& Effects)
{
	for (const FRedCEffectInfo& Effect : Effects)
	{
		if (Effect.EffectClass)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Effect.EffectClass, Effect.EffectLevel, MakeEffectContext());

			ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void URedCAbilitySystemComponent::SendProjectileSpawnEvent(const FGameplayTag EventTag, const FRedCProjectileInfo& ProjectileInfo)
{
	if (!EventTag.IsValid())
	{
		return;
	}

	// Client to Server RPC
	if (!GetAvatarActor()->HasAuthority())
	{
		ServerSendProjectileSpawnEvent(EventTag, ProjectileInfo);
		return;
	}

	// Projectile 속성 객체 생성 및 초기화 후 이벤트 발행 (Server Only)
	if (UProjectileSpawnEventObject* EventObject = NewObject<UProjectileSpawnEventObject>())
	{
		EventObject->ProjectileInfo = ProjectileInfo;

		FGameplayEventData Payload;
		Payload.OptionalObject = EventObject;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), EventTag, Payload);
	}
}

void URedCAbilitySystemComponent::ServerSendProjectileSpawnEvent_Implementation(const FGameplayTag EventTag, const FRedCProjectileInfo& ProjectileInfo)
{
	SendProjectileSpawnEvent(EventTag, ProjectileInfo);
}
