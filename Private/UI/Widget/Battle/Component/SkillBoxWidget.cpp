// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/Component/SkillBoxWidget.h"

#include "Components/OverlaySlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WrapBoxSlot.h"
#include "Types/RedCLogChannels.h"
#include "UI/Widget/_Base/Component/BaseBattleTextBlock.h"


void USkillBoxWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	for (UWidget* Child : Left_SkillBoxes->GetAllChildren())
	{
		if (UBaseSkillBox* Box = Cast<UBaseSkillBox>(Child))
		{
			if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(Box->Overlay_Slot->Slot))
				VSlot->SetHorizontalAlignment(HAlign_Right);
			Box->Text_SkillName->SetJustification(ETextJustify::Right);
		}
	}

	for (UWidget* Child : Right_SkillBoxes->GetAllChildren())
	{
		if (UBaseSkillBox* Box = Cast<UBaseSkillBox>(Child))
		{
			if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(Box->Overlay_Slot->Slot))
				VSlot->SetHorizontalAlignment(HAlign_Left);
			Box->Text_SkillName->SetJustification(ETextJustify::Left);
		}
	}
}

void USkillBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (UWidget* Child : Left_SkillBoxes->GetAllChildren())
	{
		if (UBaseSkillBox* Box = Cast<UBaseSkillBox>(Child))
		{
			if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(Box->Overlay_Slot->Slot))
				VSlot->SetHorizontalAlignment(HAlign_Right);
			Box->Text_SkillName->SetJustification(ETextJustify::Right);
		}
	}

	for (UWidget* Child : Right_SkillBoxes->GetAllChildren())
	{
		if (UBaseSkillBox* Box = Cast<UBaseSkillBox>(Child))
		{
			if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(Box->Overlay_Slot->Slot))
				VSlot->SetHorizontalAlignment(HAlign_Left);
			Box->Text_SkillName->SetJustification(ETextJustify::Left);
		}
	}
}

void USkillBoxWidget::SetBattleViewModelBinding(UBattleViewModel* ViewModel)
{
	if (ViewModel)
	{
		for (auto CharacterSkill :ViewModel->CharacterSkillInfo)
		{
			TArray<FString> AbilityTagsArray;
			
			CharacterSkill.Key.GetTagName().ToString().ParseIntoArray(AbilityTagsArray, TEXT("."));
			ECharacterAttackType AttackType = ECharacterAttackTypeHelper::StringToAttackType(AbilityTagsArray.Last());
			
			switch (AttackType)
			{
			case ECharacterAttackType::None:
				{
					UE_LOG(LogRedCWidget, Warning, TEXT("No Skill Type(None) : %d"), AttackType);
				}
				break;
			case ECharacterAttackType::EAttack:
				{
					SlotBox_E->Box_Total->SetVisibility(ESlateVisibility::Visible);
					SlotBox_E->Text_SkillName->SetText(FText::FromName(CharacterSkill.Value.SkillName));
				}
				break;
			case ECharacterAttackType::Grab:
				{
					SlotBox_F->Box_Total->SetVisibility(ESlateVisibility::Visible);
					SlotBox_F->Text_SkillName->SetText(FText::FromName(CharacterSkill.Value.SkillName));
				}
				break;
			case ECharacterAttackType::Spacebar:
				{
					SlotBox_Space->Box_Total->SetVisibility(ESlateVisibility::Visible);
					SlotBox_Space->Text_SkillName->SetText(FText::FromName(CharacterSkill.Value.SkillName));
				}
				break;
			case ECharacterAttackType::SRAttack:
				{
					SlotBox_ShiftRightMouse->Box_Total->SetVisibility(ESlateVisibility::Visible);
					SlotBox_ShiftRightMouse->Text_SkillName->SetText(FText::FromName(CharacterSkill.Value.SkillName));
				}
				break;
			case ECharacterAttackType::RAttack:
				{
					SlotBox_RightMouse->Box_Total->SetVisibility(ESlateVisibility::Visible);
					SlotBox_RightMouse->Text_SkillName->SetText(FText::FromName(CharacterSkill.Value.SkillName));
				}
				break;
			case ECharacterAttackType::SLAttack:
				{
					SlotBox_ShiftLeftMouse->Box_Total->SetVisibility(ESlateVisibility::Visible);
					SlotBox_ShiftLeftMouse->Text_SkillName->SetText(FText::FromName(CharacterSkill.Value.SkillName));
				}
				break;
				default:
					{
						UE_LOG(LogRedCWidget, Warning, TEXT("No Skill Type : %d"),AttackType);
					}
				break;
			}
		}
		// CoolDown Binding
		ViewModel->OnStartedSkillCoolTime.AddUObject(this, &ThisClass::OnStartedSkillCoolTime);
	}
}

void USkillBoxWidget::OnStartedSkillCoolTime(ECharacterAttackType SkillName, int CooldownTime)
{
	if (CooldownTime > 0)
	{
		switch (SkillName)
		{
		case ECharacterAttackType::None:
			{
				UE_LOG(LogRedCWidget, Warning, TEXT("no SkillName"));
			}
			break;
		case ECharacterAttackType::EAttack:
			{
				SlotBox_E->StartCoolTime(CooldownTime);
			}
			break;
		case ECharacterAttackType::Grab:
			{
				SlotBox_F->StartCoolTime(CooldownTime);
			}
			break;
		case ECharacterAttackType::Spacebar:
			{
				SlotBox_Space->StartCoolTime(CooldownTime);
			}
			break;
		case ECharacterAttackType::SRAttack:
			{
				SlotBox_ShiftRightMouse->StartCoolTime(CooldownTime);
			}
			break;
		case ECharacterAttackType::RAttack:
			{
				SlotBox_RightMouse->StartCoolTime(CooldownTime);
			}
			break;
		case ECharacterAttackType::SLAttack:
			{
				SlotBox_ShiftLeftMouse->StartCoolTime(CooldownTime);
			}
			break;
		default:
			{
				UE_LOG(LogRedCWidget, Warning, TEXT("no SkillName") );
			}
			break;
		}
	}
}
