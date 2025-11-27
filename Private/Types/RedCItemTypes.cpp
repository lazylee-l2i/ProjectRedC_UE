#include "Types/RedCItemTypes.h"

#include "Types/RedCLogChannels.h"


void FRedCItemArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	// AddedIndices = 추가된 아이템들의 인덱스
	// FinalSize = 배열의 최종크기
	//UE_LOG(LogRedCWidget, Warning,TEXT("[ItemTypes] PostReplicatedAdd Call"));
	for (int32 index : AddedIndices)
	{
		FRedCItemInfo& AddedItem = Items[index];
		// UE_LOG(LogRedCWidget, Error, TEXT("[ItemTypes] PostReplicatedAdd Call For: ID=[%d], Name=[%s], Level=[%d]"),
		// AddedItem.ReplicationID,
		// *AddedItem.ItemName, // 아이템 이름 (FString으로 변환)
		// AddedItem.ItemLevel);
	}
	
}

void FRedCItemArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	//UE_LOG(LogRedCWidget, Warning,TEXT("[ItemTypes] PostReplicatedChange Call"));
	for (int32 index : ChangedIndices)
	{
		FRedCItemInfo& ChangedItem = Items[index];
		OnItemLevelChanged.Broadcast(ChangedItem.ItemID, ChangedItem.ItemLevel);
		
		// UE_LOG(LogRedCWidget, Warning, TEXT("[ItemTypes] PostReplicatedChange Call For: ID=[%d], Name=[%s], Level=[%d]"),
		// ChangedItem.ReplicationID,
		// *ChangedItem.ItemName,
		// ChangedItem.ItemLevel);
	}
}

void FRedCItemArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	//remove
}

