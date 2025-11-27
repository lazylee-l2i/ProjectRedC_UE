#pragma once

#include "CoreMinimal.h"
#include "RedCCharacterSelectionInfo.generated.h"

USTRUCT()
struct FRedCCharacterSelectionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int TeamID = -1;
	
	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerController = nullptr;
	
	UPROPERTY()
	int CharacterID = -1;
};
