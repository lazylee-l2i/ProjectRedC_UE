// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "ItemShopComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREDC_API UItemShopComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UItemShopComponent();
	
	void BindInputActions(UInputComponent* PlayerInputComponent);
	void ShopPageChange();
	void OnItemBuyKeyPressed(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_SendItemBuyEvent(int32 InputNumKey);
};
