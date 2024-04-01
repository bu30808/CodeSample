// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/99_Drag/DraggableWidget.h"

#include "03_Widget/99_Drag/DragAndDropOperation.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/KismetInputLibrary.h"
#include "Logging/StructuredLog.h"

class AUserController;

UDraggableWidget::UDraggableWidget()
{
	DropIgnoreList.Add(GetClass());
}

void UDraggableWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	//모든 드래그 할 위젯의 비지빌리티는 Visible이어야만 합니다.
	SetVisibility(ESlateVisibility::Visible);
}

void UDraggableWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UDraggableWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOGFMT(LogTemp, Log, "NativeOnMouseButtonDown : {0}", GetName());
	//드래그 시작지점을 저장합니다.
	DragOffset = USlateBlueprintLibrary::AbsoluteToLocal(InGeometry, InMouseEvent.GetScreenSpacePosition());

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

FReply UDraggableWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOGFMT(LogTemp, Log, "NativeOnMouseButtonUp {0}", GetName());
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UDraggableWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                            UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	UE_LOGFMT(LogTemp, Log, "NativeOnDragDetected");
	if (bUseDrag)
	{
		OutOperation = WidgetDragDetected();
	}
}

bool UDraggableWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                    UDragDropOperation* InOperation)
{
	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);
		return WidgetMoveToDraggedPoint(oper, InGeometry, InDragDropEvent);
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UDraggableWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);
		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
	}
}

UDragAndDropOperation* UDraggableWidget::WidgetDragDetected()
{
	if (auto DragOper =
		Cast<UDragAndDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(DragDropOperationObject)))
	{
		DragOper->DraggedWidget = this;
		DragOper->DraggedWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		DragOper->OriginalOpacity = GetRenderOpacity();

		if (bUseCustomDragShadowWidget)
		{
			DragOper->DefaultDragVisual = CreateCustomDragShadowWidget();
		}
		else
		{
			DragOper->DefaultDragVisual = this;
			DragOper->DefaultDragVisual->SetRenderOpacity(0.5f);
		}


		DragOper->Pivot = EDragPivot::MouseDown;
		DragOper->DragOffset = DragOffset;

		DragOper->DesiredSize = GetDesiredSize();

		return DragOper;
	}

	return nullptr;
}
