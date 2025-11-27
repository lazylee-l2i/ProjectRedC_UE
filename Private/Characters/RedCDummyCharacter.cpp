// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RedCDummyCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


ARedCDummyCharacter::ARedCDummyCharacter()
{
	NetUpdateFrequency = 100.f;
	MinNetUpdateFrequency = 66.f;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void ARedCDummyCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARedCDummyCharacter, AbilitySystemComponent);
}

void ARedCDummyCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystemComponent();
}

void ARedCDummyCharacter::InitAbilitySystemComponent()
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (HasAuthority() && not Abilities.IsEmpty())
		{
			for (const auto& Ability : Abilities)
			{
				FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability);
				AbilitySystemComponent->GiveAbility(Spec);
			}
		}
	}
}

void ARedCDummyCharacter::OnRep_AbilitySystemComponent()
{
	InitAbilitySystemComponent();
}

UAbilitySystemComponent* ARedCDummyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

