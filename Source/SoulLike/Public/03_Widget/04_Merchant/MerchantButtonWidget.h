// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MerchantWidget.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MerchantButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UMerchantButtonWidget : public UDraggableWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Item;
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Background;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemName;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextBlock_Count;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextBlock_Price;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USimpleToolTipWidget> ToolTipWidgetObject;

	//이 버튼이 기억하고 있는 상점 아이템 데이터
	UPROPERTY()
	TWeakObjectPtr<class UBaseMerchandiseListData> Data;

	UPROPERTY(EditAnywhere)
	FSlateColor HoveredColor = FColor::Orange;
	UPROPERTY(EditAnywhere)
	FSlateColor NormalColor = FColor::White;


	virtual void NativeConstruct() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;


	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
public:
	UBaseMerchandiseListData* GetMerchandiseData() const { return Data.Get(); }
};
