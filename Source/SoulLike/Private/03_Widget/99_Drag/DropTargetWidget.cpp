// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/99_Drag/DropTargetWidget.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Logging/StructuredLog.h"


bool UDropTargetWidget::IsDropIgnoreWidget(const TWeakObjectPtr<UDraggableWidget>& droppedWidget)
{
	if (droppedWidget.IsValid())
	{
		auto find = DropIgnoreList.FindByPredicate([droppedWidget](const TSubclassOf<UDraggableWidget>& element)
		{
			return droppedWidget->IsA(element);
		});

		if (find != nullptr)
		{
			return true;
		}
	}
	return false;
}

bool UDropTargetWidget::IsDropAllowWidget(const TWeakObjectPtr<UDraggableWidget>& droppedWidget)
{
	if (droppedWidget.IsValid())
	{
		auto find = DropAllowList.FindByPredicate([droppedWidget](const TSubclassOf<UDraggableWidget>& element)
		{
			return droppedWidget->IsA(element);
		});

		if (find != nullptr)
		{
			return true;
		}
	}

	return false;
}


bool UDropTargetWidget::WidgetMoveToDraggedPoint(UDragAndDropOperation* Oper, const FGeometry& InGeometry,
                                                 const FDragDropEvent& InDragDropEvent)
{
	if (Oper)
	{
		Oper->DraggedWidget->SetRenderOpacity(Oper->OriginalOpacity);

		if (!IsDropIgnoreWidget(Oper->DraggedWidget))
		{
			//오퍼레이션으로 가져온 드래그중인 위젯을 잠깐 제거합니다.
			Oper->DraggedWidget->RemoveFromParent();
			//다시 보여줍니다.
			Oper->DraggedWidget->AddToViewport();

			Oper->DraggedWidget->SetPositionInViewport(
				UWidgetLayoutLibrary::GetMousePositionOnViewport(this) - Oper->DragOffset, false);
		}
	}


	return true;
}

bool UDropTargetWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                     UDragDropOperation* InOperation)
{
	if (auto oper = Cast<UDragAndDropOperation>(InOperation))
	{
		UE_LOGFMT(LogTemp, Log, "{0} 위젯에 {1} 위젯이 드롭되었습니다.", GetName(), oper->DraggedWidget->GetName());
		oper->DraggedWidget->SetRenderOpacity(oper->OriginalOpacity);
		oper->DraggedWidget->SetVisibility(ESlateVisibility::Visible);
		return WidgetMoveToDraggedPoint(oper, InGeometry, InDragDropEvent);
	}


	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
