// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/ActionComponent.h"

#include "Characters/RedCHeroCharacter.h"
#include "Characters/RedCPlayerCharacter.h"
#include "Characters/Components/MovementEffectManagerComponent.h"
#include "Controllers/RedCPlayerController.h"
#include "GAS/RedCPlayerAbilitySystemComponent.h"
#include "Input/RedCEnhancedInputComponent.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"
#include "Utility/LogHelper.h"


UActionComponent::UActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActionComponent::BindInputActions(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const ARedCPlayerCharacter* PlayerCharacter = GetOwner<ARedCPlayerCharacter>();
	const ARedCPlayerController* PlayerController = PlayerCharacter->GetController<ARedCPlayerController>();
	if (!PlayerCharacter || !PlayerController)
	{
		return;
	}

	const UInputConfig* InputConfig = PlayerController->GetInputConfig();
	if (!InputConfig)
	{
		return;
	}
	
	if (URedCEnhancedInputComponent* InputComponent = Cast<URedCEnhancedInputComponent>(PlayerInputComponent))
	{
		// Native Input Action Binding
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_Look, ETriggerEvent::Triggered, this, &ThisClass::LookAction);
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_Move, ETriggerEvent::Triggered, this, &ThisClass::MoveAction);
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_Move, ETriggerEvent::Completed, this, &ThisClass::MoveDoneAction);
		
		// Native Input Action Binding (Boolean)
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_LMouse, ETriggerEvent::Started, this, &ThisClass::LMouseAction);
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_LMouse, ETriggerEvent::Completed, this, &ThisClass::LMouseAction);
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_RMouse, ETriggerEvent::Started, this, &ThisClass::RMouseAction);
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_RMouse, ETriggerEvent::Completed, this, &ThisClass::RMouseAction);
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_LShift, ETriggerEvent::Started, this, &ThisClass::LShiftAction);
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_LShift, ETriggerEvent::Completed, this, &ThisClass::LShiftAction);

		if (Cast<ARedCHeroCharacter>(PlayerCharacter))
		{
			// Ability Input Action Binding (Boolean)
			InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
		}
	}
}

#pragma region Native Input Methods
void UActionComponent::LookAction(const FInputActionValue& InputActionValue)
{
	ARedCPlayerCharacter* PlayerCharacter = GetOwner<ARedCPlayerCharacter>();
	if (!PlayerCharacter)
	{
		return;
	}

	if (const UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent())
	{
		if (ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Lock_Look))
		{
			return;
		}
	}

	const FVector2D Input = InputActionValue.Get<FVector2D>();

	if (Input.X != 0.f)
	{
		PlayerCharacter->AddControllerYawInput(Input.X);
	}

	if (Input.Y != 0.f)
	{
		PlayerCharacter->AddControllerPitchInput(Input.Y);
	}
}

void UActionComponent::MoveAction(const FInputActionValue& InputActionValue)
{
#pragma region Validation Check Logic
	ARedCPlayerCharacter* PlayerCharacter = GetOwner<ARedCPlayerCharacter>();
	if (not PlayerCharacter)
	{
		return;
	}
	
	const AController* Controller = PlayerCharacter->GetController();
	if (!Controller)
	{
		return;
	}
#pragma endregion

	const FVector2D Input = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
	
	if (ARedCHeroCharacter* HeroCharacter = Cast<ARedCHeroCharacter>(PlayerCharacter))
	{
		if (const UAbilitySystemComponent* ASC = HeroCharacter->GetAbilitySystemComponent())
		{
			if (Input.Y > 0.f && not HeroCharacter->IsPressingForward())
			{
				HeroCharacter->SetForwardInput(true);
			}
			else if (Input.Y < 0.f && not HeroCharacter->IsPressingBackward())
			{
				HeroCharacter->SetBackwardInput(true);
			}

			if (ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death) ||
				ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Combat_KnockedDown) ||
				ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Lock_Move))
			{
				return;
			}
		
			if (auto* MovementEffectComp = HeroCharacter->FindComponentByClass<UMovementEffectManagerComponent>())
			{
				MovementEffectComp->ApplyMovementEffect(Input);
			}
		}
	}

	if (Input.X != 0.f)
	{
		const FVector Direction = Rotation.RotateVector(FVector::RightVector);
		PlayerCharacter->AddMovementInput(Direction, Input.X);
	}

	if (Input.Y != 0.f)
	{
		const FVector Direction = Rotation.RotateVector(FVector::ForwardVector);
		PlayerCharacter->AddMovementInput(Direction, Input.Y);
	}
}

void UActionComponent::MoveDoneAction()
{
	if (ARedCHeroCharacter* HeroCharacter = GetOwner<ARedCHeroCharacter>())
	{
		HeroCharacter->ServerRemoveMovementEffect();
		HeroCharacter->SetForwardInput(false);
		HeroCharacter->SetBackwardInput(false);
	}
}

void UActionComponent::LShiftAction(const FInputActionValue& InputActionValue)
{
	bIsLShiftPressed = InputActionValue.Get<bool>();
	
	if (not bIsLShiftPressed && bIsLMousePressed)
	{
		AbilityInputReleased(InputTags::Input_Action_Ability_SLAttack);
	}

	if (not bIsLShiftPressed && bIsRMousePressed)
	{
		AbilityInputReleased(InputTags::Input_Action_Ability_SRAttack);
	}
}

void UActionComponent::LMouseAction(const FInputActionValue& InputActionValue)
{
	bIsLMousePressed = InputActionValue.Get<bool>();
	
	if (bIsLMousePressed)
	{
		if (bIsLShiftPressed)
		{
			AbilityInputPressed(InputTags::Input_Action_Ability_SLAttack);
		}

		AbilityInputPressed(InputTags::Input_Action_Ability_LAttack);
	}
	else
	{
		if (bIsLShiftPressed)
		{
			AbilityInputReleased(InputTags::Input_Action_Ability_SLAttack);
		}

		AbilityInputReleased(InputTags::Input_Action_Ability_LAttack);
	}
}

void UActionComponent::RMouseAction(const FInputActionValue& InputActionValue)
{
	bIsRMousePressed = InputActionValue.Get<bool>();

	if (bIsRMousePressed)
	{
		if (bIsLShiftPressed)
		{
			AbilityInputPressed(InputTags::Input_Action_Ability_SRAttack);
		}
		
		AbilityInputPressed(InputTags::Input_Action_Ability_RAttack);
	}
	else
	{
		if (bIsLShiftPressed)
		{
			AbilityInputReleased(InputTags::Input_Action_Ability_SRAttack);
		}
		
		AbilityInputReleased(InputTags::Input_Action_Ability_RAttack);
	}
}
#pragma endregion

#pragma region Ability Input Methods
void UActionComponent::AbilityInputPressed(FGameplayTag InputTag)
{
	const ARedCHeroCharacter* PlayerCharacter = GetOwner<ARedCHeroCharacter>();
	if (not PlayerCharacter)
	{
		return;
	}

	if (URedCPlayerAbilitySystemComponent* ASC = PlayerCharacter->GetRedCPlayerAbilitySystemComponent())
	{
		if (not ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death) && not ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Combat_KnockedDown))
		{
			ASC->AbilityInputPressed(InputTag);	
		}
		
		//LogHelper::Print(LogRedC, ELogVerbosity::Warning, FString::Printf(TEXT("AbilityInputPressed : %s"), *InputTag.GetTagName().ToString()));
	}
}

void UActionComponent::AbilityInputReleased(FGameplayTag InputTag)
{
	const ARedCHeroCharacter* PlayerCharacter = GetOwner<ARedCHeroCharacter>();
	if (not PlayerCharacter)
	{
		return;
	}

	if (URedCPlayerAbilitySystemComponent* ASC = PlayerCharacter->GetRedCPlayerAbilitySystemComponent())
	{
		if (not ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death) && not ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Combat_KnockedDown))
		{
			ASC->AbilityInputReleased(InputTag);	
		}
		
		//LogHelper::Print(LogRedC, ELogVerbosity::Warning, FString::Printf(TEXT("AbilityInputReleased : %s"), *InputTag.GetTagName().ToString()));
	}
}
#pragma endregion
