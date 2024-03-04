// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "DropTargetWidget.generated.h"

/**
 * 이 클래스를 상속받은 위젯은 타 위젯의 드롭을 받습니다.
 */
UCLASS()
class SOULLIKE_API UDropTargetWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

protected:
	//드롭처리를 무시할 클래스의 리스트. 이 리스트에 있는 클래스가 드롭된 경우 아무 작업도 하지 않습니다.
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UDraggableWidget>> DropIgnoreList;

	//이 리스트에 있는 대상만 드롭 처리를 합니다.
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UDraggableWidget>> DropAllowList;

public:
	/**
	 * @brief 받아온 대상이 드롭 무시 대상인지 확인합니다.
	 * @param droppedWidget 
	 * @return 무시 대상이면 참.
	 */
	bool IsDropIgnoreWidget(const TWeakObjectPtr<UDraggableWidget>& droppedWidget);

	/**
	 * @brief 받아온 대상이 드롭 처리를 허용하는 대상인가 확인합니다.
	 * @param droppedWidget 
	 * @return 허용 대상이면 참.
	 */
	bool IsDropAllowWidget(const TWeakObjectPtr<UDraggableWidget>& droppedWidget);


	//오퍼레이션으로 드래그중인 위젯을 받아와 해당 지점으로 이동시킵니다.
	virtual bool WidgetMoveToDraggedPoint(class UDragAndDropOperation* Oper,
	                                      const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent);


	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;
};
