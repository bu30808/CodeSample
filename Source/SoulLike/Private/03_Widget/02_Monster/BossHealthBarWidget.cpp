// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/02_Monster/BossHealthBarWidget.h"

#include "00_Character/03_Monster/BaseMonster.h"
#include "Components/TextBlock.h"

void UBossHealthBarWidget::SetInfo(class ABaseMonster* Boss)
{
	if (Boss)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		TextBlock_Name->SetText(Boss->GetMonsterName());

		if (auto attComp = Boss->GetAttributeComponent())
		{
			attComp->OnDamagedHP.AddUniqueDynamic(this,&UBossHealthBarWidget::ShowDamage);
			attComp->OnChangeHPValue.AddUniqueDynamic(this, &UBossHealthBarWidget::OnChangeHPValueEvent);
			attComp->OnChangeHPValue.Broadcast(attComp->GetHP(), attComp->GetMaxHP());
		}
	}
}

void UBossHealthBarWidget::OnChangeHPValueEvent(float Value, float MaxValue)
{
	UpdateProgress(Value, MaxValue);
}
