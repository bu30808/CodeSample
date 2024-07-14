// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "03_Widget/01_Menu/02_QuickSlot/QuickSlotButtonWidget.h"
#include "QuickSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UQuickSlotWidget : public UDraggableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UMainItemQuickSlotButtonWidget* UMG_ItemQuickSlotButton;
	UPROPERTY(meta=(BindWidget))
	class UQuickSlotButtonWidget* UMG_AbilitySlotButton;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

};
