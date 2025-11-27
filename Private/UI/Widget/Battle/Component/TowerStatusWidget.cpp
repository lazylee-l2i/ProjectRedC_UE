// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/Component/TowerStatusWidget.h"

#include "Components/Image.h"
#include "Types/RedCLogChannels.h"


void UTowerStatusWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Img_Tower_1)
	{
		Img_Tower_1->SetBrush(Icon_Tower_1);
	}
	if (Img_BrokenTower_1)
	{
		Img_BrokenTower_1->SetBrush(Icon_BrokenTower_1);
	}
	if (Img_Tower_2)
	{
		Img_Tower_2->SetBrush(Icon_Tower_2);
	}
	if (Img_BrokenTower_2)
	{
		Img_BrokenTower_2->SetBrush(Icon_BrokenTower_2);
	}
}

void UTowerStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Img_Tower_1)
	{
		Img_Tower_1->SetBrush(Icon_Tower_1);
	}
	if (Img_BrokenTower_1)
	{
		Img_BrokenTower_1->SetBrush(Icon_BrokenTower_1);
	}
	if (Img_Tower_2)
	{
		Img_Tower_2->SetBrush(Icon_Tower_2);
	}
	if (Img_BrokenTower_2)
	{
		Img_BrokenTower_2->SetBrush(Icon_BrokenTower_2);
	}
}

void UTowerStatusWidget::SetTowerCount(int InputTowerCount)
{
	TowerCount = InputTowerCount;

	TowerIconChange();
}


void UTowerStatusWidget::TowerIconChange()
{
	switch (TowerCount)
	{
		case 0:
		{
				WidgetSwitcher_Tower1->SetActiveWidgetIndex(1);
				WidgetSwitcher_Tower2->SetActiveWidgetIndex(1);
			break;
		}
		case 1:
			{
				WidgetSwitcher_Tower1->SetActiveWidgetIndex(1);
				WidgetSwitcher_Tower2->SetActiveWidgetIndex(0);
				break;
			}
		case 2:
			{
				WidgetSwitcher_Tower1->SetActiveWidgetIndex(0);
				WidgetSwitcher_Tower2->SetActiveWidgetIndex(0);
				break;
			}
		default:
		{
			UE_LOG(LogRedCWidget, Error, TEXT("TowerIconChange Func Error / TowerCount : %d"), TowerCount);
		}
	}
}