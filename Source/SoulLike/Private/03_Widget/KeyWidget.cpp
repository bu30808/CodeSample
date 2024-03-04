// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/KeyWidget.h"

#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"

void UKeyWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetKeyAndAction(KeyName, ActionName);
}

void UKeyWidget::SetKeyAndAction(FName Key, FString Action)
{
	TextBlock_Key->SetText(FText::FromName(Key));
	TextBlock_Action->SetText(FText::FromString(Action));
}
