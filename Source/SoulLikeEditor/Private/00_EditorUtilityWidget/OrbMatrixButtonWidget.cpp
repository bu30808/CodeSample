// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/OrbMatrixButtonWidget.h"

#include "00_EditorUtilityWidget/OrbCreatorWidget.h"
#include "95_OrbCreator/OrbMatrix.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Logging/StructuredLog.h"

void UOrbMatrixButtonWidget::ChangeButtonStyle(UTexture2D* NewTexture)
{
	SetLocked(bLock);

	if (ColorForSlotType.Contains(OrbMatrixSlotType))
	{
		auto style = Button->GetStyle();

		style.Normal.TintColor = ColorForSlotType[OrbMatrixSlotType];
		style.Normal.SetResourceObject(NewTexture);

		style.Hovered.TintColor = FLinearColor(0.095307, 0.095307, 0.095307, 1.000000);
		style.Hovered.SetResourceObject(NewTexture);

		style.Pressed.TintColor = FLinearColor(0.028426, 0.028426, 0.028426, 1.000000);
		style.Pressed.SetResourceObject(NewTexture);

		Button->SetStyle(style);
	}
}

void UOrbMatrixButtonWidget::OnClickedButton()
{
	if (OrbCreatorWidget.IsValid())
	{
		OrbCreatorWidget->CurClickedButton = this;
		OrbCreatorWidget->SetButtonInfo(this);
	}
}

void UOrbMatrixButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Button)
	{
		Button->OnClicked.AddUniqueDynamic(this, &UOrbMatrixButtonWidget::OnClickedButton);
	}
}

void UOrbMatrixButtonWidget::SetColRow(int32 R, int32 C)
{
	Row = R;
	Col = C;
}

void UOrbMatrixButtonWidget::SetCore()
{
	Button->SetVisibility(ESlateVisibility::HitTestInvisible);
	OrbMatrixSlotType = EOrbMatrixSlotType::CORE;
	ChangeButtonStyle(PointSlotTexture);
	LineDirection = EOrbMatrixSlotLineDirection::None;
	SlotContent = EOrbMatrixSlotContent::Set;
	bLock = false;
}

void UOrbMatrixButtonWidget::SetLine(EOrbMatrixSlotLineDirection OrbMatrixSlotLineDirection)
{
	LineDirection = OrbMatrixSlotLineDirection;

	if (LineSlotTexture)
	{
		ChangeButtonStyle(LineSlotTexture);

		switch (OrbMatrixSlotLineDirection)
		{
		case EOrbMatrixSlotLineDirection::Vertical:
			Button->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.f, 1.f),
			                                            FVector2D::ZeroVector, 90.f));
			break;
		case EOrbMatrixSlotLineDirection::Horizontal:
			Button->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.f, 1.f),
			                                            FVector2D::ZeroVector, 0.f));
			break;
		case EOrbMatrixSlotLineDirection::LeftUpToRightDown:
			Button->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.8f, 1.f),
			                                            FVector2D::ZeroVector, 45.f));
			break;
		case EOrbMatrixSlotLineDirection::RightUpToLeftDown:
			Button->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.8f, 1.f),
			                                            FVector2D::ZeroVector, -45.f));
			break;
		case EOrbMatrixSlotLineDirection::None:
			Button->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.f, 1.f),
			                                            FVector2D::ZeroVector, 0.f));
			ChangeButtonStyle(nullptr);
			break;
		default: ;
		}
	}
}

void UOrbMatrixButtonWidget::SetPoint(EOrbMatrixSlotType NewOrbMatrixSlotType)
{
	LineDirection = EOrbMatrixSlotLineDirection::None;
	OrbMatrixSlotType = NewOrbMatrixSlotType;
	Button->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.f, 1.f), FVector2D::ZeroVector,
	                                            0.f));

	switch (OrbMatrixSlotType)
	{
	case EOrbMatrixSlotType::NONE:
		SetRenderScale(FVector2D(1));
		ChangeButtonStyle(nullptr);
		break;
	case EOrbMatrixSlotType::LINE:
		ChangeButtonStyle(LineSlotTexture);
		SetLine(EOrbMatrixSlotLineDirection::Horizontal);
		break;
	case EOrbMatrixSlotType::CORE:
		SetCore();
	case EOrbMatrixSlotType::DEFENCE:
	case EOrbMatrixSlotType::PHYSICAL:
	case EOrbMatrixSlotType::MAGICAL:
	case EOrbMatrixSlotType::FREE:
		ChangeButtonStyle(PointSlotTexture);
		break;
	default: ;
	}
}

void UOrbMatrixButtonWidget::ForceUpdate(EOrbMatrixSlotLineDirection SlotLineDirection,
                                         EOrbMatrixSlotType OrbMatrixSlot, bool bLocked)
{
	/*UE_LOGFMT(LogTemp, Log, "버튼 설정 : {0}/{1}", StaticEnum<EOrbMatrixSlotType>()->GetValueAsString(OrbMatrixSlot),
	          StaticEnum<EOrbMatrixSlotLineDirection>()->GetValueAsString(SlotLineDirection));*/

	if (OrbMatrixSlot == EOrbMatrixSlotType::NONE || OrbMatrixSlot == EOrbMatrixSlotType::CORE || OrbMatrixSlot ==
		EOrbMatrixSlotType::LINE)
	{
		bLock = false;
	}
	else
	{
		bLock = bLocked;
	}

	SetPoint(OrbMatrixSlot);
	SetLine(SlotLineDirection);
}

void UOrbMatrixButtonWidget::SetLocked(bool bIsLocked)
{
	bLock = bIsLocked;
	if (bLock)
	{
		//잠금표시
		Image_Lock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		//잠금 아님 표시
		Image_Lock->SetVisibility(ESlateVisibility::Collapsed);
	}
}
