// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Battle/OverheadHealthWidget.h"

#include "GAS/Attributes/RedCHealthSet.h"
#include "UI/Widget/_Base/Component/BaseOverheadGauge.h"


void UOverheadHealthWidget::BindHealthAttribute(UAbilitySystemComponent* ASC)
{
	HealthGauge->BindGameplayAttribute(ASC, URedCHealthSet::GetHealthAttribute(), URedCHealthSet::GetMaxHealthAttribute());
}

void UOverheadHealthWidget::SetTeamColor(bool bIsSameTeam)
{
	HealthGauge->SetGaugeColor(TeamColor[bIsSameTeam]);
}
