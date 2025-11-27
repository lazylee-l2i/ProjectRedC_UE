// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Frameworks/RedCPlayerStateBase.h"
#include "PlayerStateLobby.generated.h"


/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchingCompleted, bool);

UCLASS()
class PROJECTREDC_API APlayerStateLobby : public ARedCPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	FOnMatchingCompleted OnMatchingCompleted;

	UPROPERTY(ReplicatedUsing=OnRep_MatchingCompleted)
	bool MatchingCompleted = false;
	
	UPROPERTY(Replicated)
	int TeamIDForLobby = -1;

	UFUNCTION()
	void OnRep_MatchingCompleted();
};
