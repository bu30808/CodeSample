// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DropTargetWidget.h"
#include "03_Widget/99_Drag/DragAndDropOperation.h"
#include "DraggableWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UDraggableWidget : public UDropTargetWidget
{
	GENERATED_BODY()

public:
	UDraggableWidget();

protected:
	//이 값이 거짓이면 Visiblity를 변경하여 더 이상 드래그 되지 않도록 합니다.
	UPROPERTY(EditAnywhere)
	bool bUseDrag = true;

	//드래그 시 필요한 정보를 저장할 오퍼레이터
	UPROPERTY()
	TSubclassOf<class UDragAndDropOperation> DragDropOperationObject = UDragAndDropOperation::StaticClass();


	UPROPERTY()
	FVector2D DragOffset;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;


	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	                                  UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


	UPROPERTY(EditAnywhere)
	bool bUseCustomDragShadowWidget = false;
	virtual UDragAndDropOperation* WidgetDragDetected();
	//드래그시 커스텀 위젯을 그림자로 사용하려면 bUseCustomDragShadowWidget변수를 참으로 준 뒤, 덮어쓰세요.
	UFUNCTION(BlueprintCallable)
	virtual UUserWidget* CreateCustomDragShadowWidget() { return nullptr; }
};
