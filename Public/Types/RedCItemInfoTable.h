#pragma once

#include "CoreMinimal.h"
#include "RedCItemTypes.h"
#include "Engine/DataTable.h"  
#include "RedCItemInfoTable.generated.h"

UENUM()
enum class EItemType : uint8
{
	Hand,
	Hat,
	Chest,
	Belt,
	Pants,
	Shoe,
	Neckless,
	HealPotion,
	AttackBuffPotion,
	DefenseBuffPotion
};


USTRUCT(BlueprintType)
struct PROJECTREDC_API FRedCItemInfoTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRedCItemInfo ItemInfo;
};
