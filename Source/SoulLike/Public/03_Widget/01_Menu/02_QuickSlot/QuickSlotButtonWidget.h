// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/MainWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "QuickSlotButtonWidget.generated.h"


UENUM(BlueprintType)
enum class EQuickSlotType : uint8
{
	CONSUME,
	ABILITY
};

struct FInventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegisterItem, class APlayerCharacter*, Player,
											   class UItemData*, Data, int32, Index);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegisterAbility, class APlayerCharacter*, Player,
											   class UAbilityData*, Data, int32, Index);


DECLARE_DELEGATE_RetVal_OneParam(int32,FOnRemoveAlreadyRegisteredSlotItem, class UItemData* Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveAlreadyRegisteredSlotAbility, class UAbilityData*, Data);
/**
 * 
 */
UCLASS()
class SOULLIKE_API UQuickSlotButtonWidget : public UDraggableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	class UImage* Image;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Key;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Count;


	UPROPERTY(VisibleAnywhere)
	EQuickSlotType QuickSlotType;
	UPROPERTY(EditAnywhere)
	FText KeyText;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> DefaultImage;
	
	UPROPERTY(Transient)
	TObjectPtr<class UInventoryData> InventoryData;
	UPROPERTY(Transient)
	TObjectPtr<class UInventoryComponent> InventoryComponent;
	
	//이 슬롯의 위치정보
	UPROPERTY(VisibleAnywhere)
	int32 Index = 0;
	//메인위젯에 붙어있는 퀵슬롯 위젯입니다.
	UPROPERTY(Transient)
	TWeakObjectPtr<class UQuickSlotWidget> MainQuickSlotWidget;


public:

	void SetIndex(const int32 NewIndex){ Index = NewIndex;}
	const int32& GetIndex() const {return Index;}
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	/**
	 * 버튼을 데이터 내용으로 설정합니다.
	 * @param Data 
	 * @param bIsLoaded 게임을 로드할 때 초기화 하는 경우에 참을 주세요. 그렇지 않으면 무한루프에 빠집니다.
	 */
	virtual void Init(UInventoryData* Data, bool bIsLoaded = false) { }
	UFUNCTION()
	virtual void UseQuickSlot(UInputAction* InputAction){ }

	virtual void ClearSlot();
	
	template <class T>
	T* GetSlotData();
};

template <class T>
T* UQuickSlotButtonWidget::GetSlotData()
{
	if(InventoryData)
	{
		return Cast<T>(InventoryData);
	}
	return nullptr;
}


UCLASS()
class SOULLIKE_API UItemQuickSlotButtonWidget : public UQuickSlotButtonWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(Blueprintable)
	FOnRegisterItem OnRegisterItem;
public:
	FOnRemoveAlreadyRegisteredSlotItem OnRemoveAlreadyRegisteredSlotItem;
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//슬롯에 등록된 아이템이 추가, 변경(사용)되었을 때 호출됩니다.
	UFUNCTION()
	void OnRefreshItemSlot(const FGuid& ItemUniqueID, const int32& NewCount);
	//슬롯에 등록된 아이템이 제거되었을 떄 호출됩니다.
	UFUNCTION()
	void OnSlotItemRemoved(ABaseCharacter* UsedBy, const FGuid& RemoveItemUniqueID);
	

	//이미 다른 슬롯에 등록되어진 아이템인지 확인합니다.
	bool IsAlreadyRegistered(UItemData* Data) const;
	
	virtual void Init(UInventoryData* Data, bool bIsLoaded = false) override;
	virtual void UseQuickSlot(UInputAction* InputAction) override;

public:
	virtual void ClearSlot() override;
	void RestoreSlotFromUniqueID(const FGuid& ItemID);
};

UCLASS()
class SOULLIKE_API UMainItemQuickSlotButtonWidget : public UQuickSlotButtonWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnFirstUpdateMainItemQuickSlotEvent(const UItemData* Data, int32 SlotIndex);
	UFUNCTION()
	void OnChangeItemQuickSlotEvent(const FInventoryItem& Item, int32 SlotIndex);
	UFUNCTION()
	void OnItemQuickSlotUpdateEvent(const FGuid& ItemUniqueID, const int32& NewCount);
	UFUNCTION()
	void OnRemoveItemQuickSlotEvent(const UItemData* Data, int32 SlotIndex);

	//이 슬롯이 표시하고 있는 아이템의 유니크 아이디
	UPROPERTY(Transient)
	FGuid MemorisedUniqueID;

	virtual void ClearSlot() override;
};

UCLASS()
class SOULLIKE_API UAbilityQuickSlotButtonWidget : public UQuickSlotButtonWidget
{
	GENERATED_BODY()

protected:


	UPROPERTY(Blueprintable)
	FOnRegisterAbility OnRegisterAbility;
public:
	UPROPERTY(BlueprintAssignable)
	FOnRemoveAlreadyRegisteredSlotAbility OnRemoveAlreadyRegisteredSlotAbility;
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	//슬롯에 등록된 어빌리티가 제거되었을 떄 호출됩니다.
	UFUNCTION()
	void OnSlotAbilityRemoved(ABaseCharacter* UsedBy, const FGameplayTag& AbilityTag);
	

	//이미 다른 슬롯에 등록되어진 어빌리티인지 확인합니다.
	bool IsAlreadyRegistered(UAbilityData* Data) const;
	
	virtual void Init(UInventoryData* Data, bool bIsLoaded = false) override;
	virtual void UseQuickSlot(UInputAction* InputAction) override;

};