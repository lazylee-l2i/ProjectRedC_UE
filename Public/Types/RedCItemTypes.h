#pragma once
#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "RedCItemTypes.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemLevelChanged, FName, int32) //RowItemName, Level


USTRUCT(BlueprintType)
struct FRedCItemProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemPrice = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Critical = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Evasion = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 0;
};

USTRUCT(BlueprintType)
struct FRedCItemInfo : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID = FName(); //Row Name

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemMaxLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCoolTime = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> ItemIcon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRedCItemProperty> ItemProperty = TArray<FRedCItemProperty>();

};



USTRUCT(BlueprintType)
struct FRedCItemArray : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FRedCItemInfo> Items = TArray<FRedCItemInfo>();
	
	FOnItemLevelChanged OnItemLevelChanged;
	


	// 콜백함수(자동호출)
	// virtual 없이도 템플릿이 컴파일시점에 맞는 함수를 알아서 호출
	void PostReplicatedAdd(const TArrayView< int32 > & AddedIndices, int32 FinalSize);  
	void PostReplicatedChange(const TArrayView< int32 >& ChangedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView< int32 >& RemovedIndices,int32 FinalSize);
	
	// 변경된 단일부분만 복제(기본 상속함수)
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FRedCItemInfo, FRedCItemArray>(
			Items,
			DeltaParams,
			*this
			);
	}
};

template<>
struct TStructOpsTypeTraits<FRedCItemArray> : public TStructOpsTypeTraitsBase2<FRedCItemArray>
{
	enum { WithNetDeltaSerializer = true };
};
