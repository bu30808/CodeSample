// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemListWidget.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ItemButtonWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredInventoryItem,class UInventoryData*,Data);
/**
 * 
 */
UCLASS()
class SOULLIKE_API UItemButtonWidget : public UDraggableWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
friend class UItemListWidget;
protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Item;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemName;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Count;
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Equipped;
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Background;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UItemMenuWidget> ItemMenuWidgetObject;

	//이 버튼이 어떤 타입의 아이템 리스트 위젯에서 생성되었는지 나타냅니다.
	UPROPERTY()
	EItemListType ItemListType;


	static TWeakObjectPtr<UItemMenuWidget> ItemMenuWidget;
protected:
	UPROPERTY(EditAnywhere)
	FLinearColor NormalColor = FLinearColor::White;
	UPROPERTY(EditAnywhere)
	FLinearColor HoverColor = FLinearColor(FColor::Orange);
	UPROPERTY(EditAnywhere)
	FLinearColor PressedColor = FLinearColor::Gray;
	UPROPERTY(EditAnywhere)
	class USoundBase* HoverSound;
	UPROPERTY(EditAnywhere)
	class USoundBase* ClickSound;
	

	//이 버튼이 표시하고 있는 정보를 담은 변수입니다.
	UPROPERTY()
	TWeakObjectPtr<class UInventoryData> InventoryData;


	virtual void NativeConstruct() override;

	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void OnUseItem();
	UFUNCTION()
	void OnRemoveItem();
	UFUNCTION()
	void OnCancel();
	void ShowMenu();

	UFUNCTION()
	void OnClickedEvent();
	
	void ProcessInventoryData(UObject* ListItemObject);
	void ProcessAbilityData(UObject* ListItemObject);


	UItemMenuWidget* GetItemMenuWidget();

public:
	void RefreshItemData(class UItemListWidget* ParentsWidget);
	class UInventoryData* GetInventoryData() const { return InventoryData.Get(); }
	void SetEquipped(bool bEquip) const;
	
	//상점 아이템 버튼이 드롭되었을때 호출되는 이벤트입니다.
	FOnPlayerBuyItemFromNPC OnPlayerBuyItemFromNPC;
	FOnPlayerBuyAbilityFromNPC OnPlayerBuyAbilityFromNPC;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UPROPERTY(Blueprintable,BlueprintCallable)
	FOnHoveredInventoryItem OnHovered;
	
	UFUNCTION()
	void OnHoveredEvent();
	UFUNCTION()
	void OnUnhoveredEvent();
	
};
