// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/PlayerState/PlayerStateLobby.h"

#include "Net/UnrealNetwork.h"


void APlayerStateLobby::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APlayerStateLobby, TeamIDForLobby);
	DOREPLIFETIME(APlayerStateLobby, MatchingCompleted);
}

void APlayerStateLobby::OnRep_MatchingCompleted()
{
	OnMatchingCompleted.Broadcast(MatchingCompleted);
}
