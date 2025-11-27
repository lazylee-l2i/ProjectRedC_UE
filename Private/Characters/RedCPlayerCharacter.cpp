// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RedCPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Characters/Components/ActionComponent.h"
#include "Controllers/RedCPlayerController.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "GameFramework/SpringArmComponent.h"


ARedCPlayerCharacter::ARedCPlayerCharacter()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArmComp);

	ActionComp = CreateDefaultSubobject<UActionComponent>(TEXT("ActionComponent"));
	ItemShopComp = CreateDefaultSubobject<UItemShopComponent>(TEXT("ItemShopComponent"));
}

void ARedCPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (ARedCPlayerController* PC = GetController<ARedCPlayerController>())
	{
		PC->ClearInputMappingContext();
		PC->SetupInputMappingContext();
	}
}

void ARedCPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	ActionComp->BindInputActions(PlayerInputComponent);
	ItemShopComp->BindInputActions(PlayerInputComponent);
}

UAbilitySystemComponent* ARedCPlayerCharacter::GetAbilitySystemComponent() const
{
	return Super::GetAbilitySystemComponent();
}
