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
	class UQuickSlotButtonWidget* UMG_QuickSlotButton;
	UPROPERTY(meta=(BindWidget))
	class UQuickSlotButtonWidget* UMG_AbilitySlotButton;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnUpdateCurAbilityQuickSlotEvent(const FGameplayTag& Tag, bool bRemove, int32 SelectedIndex);
	UFUNCTION()
	void OnUpdateCurConsumeQuickSlotEvent(const FInventoryItem& Item, bool bRemove, int32 SelectedIndex);

public:
	void LoadSelectedConsumeQuickSlot(const FInventoryItem& InventoryItem);
	void LoadSelectedAbilityQuickSlot(class UAbilityBase* Ability);
};
