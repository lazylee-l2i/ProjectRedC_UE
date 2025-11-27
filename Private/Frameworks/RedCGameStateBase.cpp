// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RedCGameStateBase.h"

#include "Net/UnrealNetwork.h"

void ARedCGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARedCGameStateBase, ProjectGameMode);
}
