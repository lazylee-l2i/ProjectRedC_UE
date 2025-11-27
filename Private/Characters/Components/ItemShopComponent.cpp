// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/ItemShopComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "Characters/RedCPlayerCharacter.h"
#include "Controllers/RedCPlayerController.h"
#include "Input/RedCEnhancedInputComponent.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"
#include "UI/Manager/UIManagerSubsystem.h"


UItemShopComponent::UItemShopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemShopComponent::BindInputActions(UInputComponent* PlayerInputComponent)
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
		InputComponent->BindOnlyKey(
		InputConfig,
		this,
		&ThisClass::OnItemBuyKeyPressed
		);
		
		InputComponent->BindNativeAction(InputConfig, InputTags::Input_Action_Native_ShopSlot_Change, ETriggerEvent::Started, this, &ThisClass::ShopPageChange);
	}
}


void UItemShopComponent::OnItemBuyKeyPressed(const FInputActionValue& Value)
{
	UE_LOG(LogRedCWidget, Warning, TEXT("OnItemBuyKeyPressed"));

	// keyboard 1~8 숫자변환
	int32 InputNumKey = Value.Get<FVector2D>().X;
	
	if (const ARedCPlayerCharacter* Player = GetOwner<ARedCPlayerCharacter>())
	{
		if (ARedCPlayerController* PC = Player->GetController<ARedCPlayerController>())
		{
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				UUIManagerSubsystem* UIManager = LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
				int32 nowBattleShopPageIndex = UIManager->GetCurrentBattleShopPageIndex();

				if (nowBattleShopPageIndex == 0)
				{
					Server_SendItemBuyEvent(InputNumKey);
				}
				else if (nowBattleShopPageIndex == 1)
				{
					Server_SendItemBuyEvent(InputNumKey + 8);
				}
				else
				{
					UE_LOG(LogRedCWidget, Warning, TEXT("slot error #1"));
				}
			}
		}
	}
}


void UItemShopComponent::Server_SendItemBuyEvent_Implementation(int32 InputNumKey)
{
	if (ARedCPlayerCharacter* Player = GetOwner<ARedCPlayerCharacter>())
	{
		FGameplayEventData Payload;
		Payload.EventTag       = FGameplayTag::RequestGameplayTag(TEXT("Event.Ability.ItemBuy")); // GA 트리거와 동일
		Payload.EventMagnitude = InputNumKey; // 슬롯 번호
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, Payload.EventTag, Payload);
	}
}


void UItemShopComponent::ShopPageChange()
{
	if (ARedCPlayerCharacter* Player = Cast<ARedCPlayerCharacter>(GetOwner()))
	{
		if (ARedCPlayerController* PC = Cast<ARedCPlayerController>(Player->GetController()))
		{
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				UUIManagerSubsystem* UIManager = LocalPlayer->GetSubsystem<UUIManagerSubsystem>();
				UIManager->BattleShopPageChange();
			}
		}
	}
}
