// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/06_Enhancement/EnhancementAttributeWidget.h"

#include "03_Widget/06_Enhancement/EnhancementWidget.h"
#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UEnhancementAttributeWidget::OnChecked(bool bIsChecked)
{
	//부모 위젯한테 부탁해서 다른 체크박스의 체크를 전부 해제합니다.
	if (bIsChecked)
	{
		if (EnhancementWidget.IsValid())
		{
			EnhancementWidget->ForceUnCheckOthers(this);
		}
	}
}

void UEnhancementAttributeWidget::SetInfo()
{
	if (Images.Contains(SelectedAttribute))
	{
		Image->SetBrushFromSoftTexture(Images[SelectedAttribute]);
	}

	if (Texts.Contains(SelectedAttribute))
	{
		TextBlock_Attribute->SetText(Texts[SelectedAttribute]);
	}
}

void UEnhancementAttributeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UEnhancementAttributeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CheckBox)
	{
		CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &UEnhancementAttributeWidget::OnChecked);
	}
}

void UEnhancementAttributeWidget::SetAttributeType(EAttributeType NewAttribute)
{
	SelectedAttribute = NewAttribute;
	SetInfo();
}

void UEnhancementAttributeWidget::SetParent(UEnhancementWidget* PWidget)
{
	EnhancementWidget = PWidget;
}
