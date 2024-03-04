// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DragAndDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UDragAndDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	//드래그중인 위젯을 임시저장하는 변수입니다.
	UPROPERTY()
	TWeakObjectPtr<class UDraggableWidget> DraggedWidget;

	//얼마나 드래그 했는지 저장하는 변수입니다.
	UPROPERTY()
	FVector2D DragOffset;

	UPROPERTY()
	FVector2D DesiredSize;

	//드래그 할 때, 드래그 중인 위젯의 투명도를 저장합니다.
	UPROPERTY()
	float OriginalOpacity;
	UPROPERTY()
	FVector2D OriginalScale;
};
