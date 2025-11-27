// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/RedCGameInstance.h"

#include "Core/Systems/RedCAssetManager.h"


URedCGameInstance::URedCGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void URedCGameInstance::Init()
{
	Super::Init();
	
	URedCAssetManager::Initialize();
}

void URedCGameInstance::Shutdown()
{
	Super::Shutdown();

	URedCAssetManager::DeInitialize();
}
