// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/05_Orb/OrbElementFilterWidget.h"

#include "96_Library/ItemHelperLibrary.h"
#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"

void UOrbElementFilterWidget::OnCheckStateChangedEvent(bool bIsChecked)
{
	if (OrbElementListWidget.IsValid())
	{
		if (bIsChecked)
		{
			if (bOrbSlotFilter)
			{
				OrbElementListWidget->AddFilter(SlotType);
			}
			else
			{
				OrbElementListWidget->AddFilter(FilterType);
			}
		}
		else
		{
			if (bOrbSlotFilter)
			{
				OrbElementListWidget->RemoveFilter(SlotType);
			}
			else
			{
				OrbElementListWidget->RemoveFilter(FilterType);
			}
		}

		OrbElementListWidget->OnUpdateFilterEvent();
	}
}

void UOrbElementFilterWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (bOrbSlotFilter == false)
	{
		if (Images.Contains(FilterType))
		{
			Image->SetBrushFromSoftTexture(Images[FilterType]);
		}

		if (Texts.Contains(FilterType))
		{
			TextBlock_Item->SetText(Texts[FilterType]);
		}
	}
	else
	{
		if (SlotImage.Contains(SlotType))
		{
			Image->SetBrushFromSoftTexture(SlotImage[SlotType]);
		}

		if (SlotText.Contains(SlotType))
		{
			TextBlock_Item->SetText(SlotText[SlotType]);
		}
	}


	if (CheckBox)
	{
		CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &UOrbElementFilterWidget::OnCheckStateChangedEvent);
	}
}

void UOrbElementFilterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (CheckBox)
	{
		CheckBox->SetIsChecked(bInitCheck);
	}
}
