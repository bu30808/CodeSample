// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/07_NPC/NPCActionButtonWidget.h"

#include "Components/TextBlock.h"

void UNPCActionButtonWidget::SetActionName(const FText& NewAction)
{
	TextBlock_ActionName->SetText(NewAction);
}
