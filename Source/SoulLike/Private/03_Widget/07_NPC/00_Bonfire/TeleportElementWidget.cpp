// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/07_NPC/00_Bonfire/TeleportElementWidget.h"

#include "NiagaraValidationRule.h"
#include "03_Widget/07_NPC/00_Bonfire/BonfireTeleportWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"

void UTeleportElementWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Image_CurLocation)
	{
		Image_CurLocation->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Button_Teleport)
	{
		Button_Teleport->OnHovered.AddUniqueDynamic(this, &UTeleportElementWidget::OnHovered);
		Button_Teleport->OnClicked.AddUniqueDynamic(this, &UTeleportElementWidget::OnClicked);
		OriginalButtonStyle = Button_Teleport->GetStyle();
	}
}

void UTeleportElementWidget::SetInfo(const FBonfireInformation& Info, class UBonfireTeleportWidget* OwnerWidget)
{
	TeleportInformation = Info;

	TextBlock_LocationName->SetText(Info.LocationName);

	ParentsWidget = OwnerWidget;
}

void UTeleportElementWidget::ShowCurLocation()
{
	if (Image_CurLocation)
	{
		Image_CurLocation->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTeleportElementWidget::SetSelected()
{
	Button_Teleport->SetStyle(SelectedButtonStyle);
}

void UTeleportElementWidget::SetUnSelected()
{
	Button_Teleport->SetStyle(OriginalButtonStyle);
}


void UTeleportElementWidget::SetIsEnabled(bool bInIsEnabled)
{
	Super::SetIsEnabled(bInIsEnabled);

	if (bInIsEnabled == true)
	{
		HorizontalBox_NotFound->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		HorizontalBox_NotFound->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}


void UTeleportElementWidget::OnHovered()
{
	if (ParentsWidget.IsValid())
	{
		ParentsWidget->SetPreviewImage(TeleportInformation.LocationImage);
	}
}

void UTeleportElementWidget::OnClicked()
{
	if (ParentsWidget.IsValid())
	{
		ParentsWidget->SetSelectedInformation(this);
	}
}
