#pragma once
#include "CoreMinimal.h"
#include "Types/RedCItemTypes.h"
#include "ShopItemBoxListEntryData.generated.h"

UCLASS(BlueprintType)
class PROJECTREDC_API UShopItemBoxListEntryData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FRedCItemInfo ItemInfo;
	UPROPERTY(BlueprintReadOnly)
	int32 InputKeyText;
};