// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/_Base/Component/BaseBattleResult.h"


void UBaseBattleResult::PlayMoveDownAnim()
{
	PlayAnimation(MoveDownAnim, 0, 1, EUMGSequencePlayMode::Forward, 1);
}
