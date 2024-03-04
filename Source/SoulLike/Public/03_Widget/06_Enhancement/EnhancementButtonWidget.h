// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/01_Menu/05_Orb/OrbListButtonWidget.h"
#include "03_Widget/99_Drag/DropTargetWidget.h"
#include "Blueprint/UserWidget.h"
#include "EnhancementButtonWidget.generated.h"


UENUM(BlueprintType)
enum class EEnhancementItemType : uint8
{
	None,
	FromInventory,
	//일반적으로 인벤토리 컴포넌트 내부에 저장되는 아이템인 경우
	FromOrbFragmentHandler //오브 프래그먼트 핸들러로 저장되는 아이템인 경우
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UEnhancementButtonWidget : public UDropTargetWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemName;

	UPROPERTY()
	EEnhancementItemType EnhancementItemType = EEnhancementItemType::None;

	//이 버튼에 할당된 아이템의 유니크아이디
	UPROPERTY()
	FGuid ItemUniqueID;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> DefaultImage;
	UPROPERTY()
	TWeakObjectPtr<class UEnhancementWidget> EnhancementWidget;

	void SetInfoFromInventoryButton(const UItemButtonWidget* InventoryButton);
	void SetInfoFromOrbButton(UOrbListButtonWidget* OrbListButton);

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SetDefaultToolTip();

	virtual void NativeConstruct() override;

public:
	void Clear();
	void SetOwnerWidget(class UEnhancementWidget* E_Widget);
	void OverrideToolTip(const FInventoryItem& RefreshedItem);
};
