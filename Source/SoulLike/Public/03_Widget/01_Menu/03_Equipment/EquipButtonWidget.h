// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/99_Drag/DropTargetWidget.h"
#include "EquipButtonWidget.generated.h"

UENUM(BlueprintType)
enum class EEquipSlotType : uint8
{
	SPIRIT,
	ARMOR,
	RING,
	CORE,
	MAX
};

UENUM(BlueprintType)
enum class EEquipButtonType : uint8
{
	EQUIPMENT,
	CHARACTER_INFORMATION,
	MAX
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquipped);

/**
 * 
 */
UCLASS()
class SOULLIKE_API UEquipButtonWidget : public UDropTargetWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Name;

	UPROPERTY(EditAnywhere)
	EEquipSlotType EquipSlotType;

	UPROPERTY(EditAnywhere)
	EEquipButtonType EquipButtonType;

	UPROPERTY(EditAnywhere)
	TMap<EEquipSlotType, TSoftObjectPtr<class UTexture2D>> IconMap;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> DefaultImage;
	UPROPERTY(EditAnywhere)
	class USoundBase* EquipSound;
	//이 버튼이 표시하고 있는 아이템 데이터입니다.
	UPROPERTY()
	TWeakObjectPtr<class UItemData> ItemData;

	//이 버튼의 인덱스 번호
	UPROPERTY(EditAnywhere)
	int32 Index = 0;

	UPROPERTY()
	TWeakObjectPtr<class UEquipWidget> ParentWidget;
	
	//장착 해제 이벤트
	FOnUnEquipped OnUnEquipped;

	//이 위젯 정보를 초기화 합니다.
	void Clean();


	virtual void NativePreConstruct() override;


	//받아온 아이템 정보가 이 버튼과 일치하는지 확인합니다.
	bool IsCorrectType(UInventoryData* InventoryData) const;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
							  UDragDropOperation* InOperation) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	void UnEquip();
	void SetButtonInfo(TWeakObjectPtr<UItemData> NewData);
	void SetEquipSlotType(EEquipSlotType NewType);
	void SetEquipButtonType(EEquipButtonType NewType);
	void SetSlotIndex(int32 NewIndex){Index = NewIndex;}
	int32 GetIndex() const {return Index;}

	FGuid GetEquippedID() const;
	void SetParentsWidget(class UEquipWidget* PWidget);
	
};


/*
//캐릭터 정보창에 장비 상태를 나타내기 위해 사용하는 위젯입니다.
UCLASS()
class SOULLIKE_API UCharacterInfoEquipButton : public UEquipButtonWidget
{
	GENERATED_BODY()

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
						  UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};*/