// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/MainWidget.h"
#include "03_Widget/01_Menu/00_Inventory/ItemButtonWidget.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "QuickSlotButtonWidget.generated.h"


UENUM(BlueprintType)
enum class EQuickSlotType
{
	CONSUME,
	ABILITY
};

struct FInventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegisterItemOrAbility,class APlayerCharacter*,Player,class UInventoryData*,Data,int32,Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveAlreadyRegisteredSlot,class UInventoryData*,Data);
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


	UPROPERTY(EditAnywhere)
	EQuickSlotType QuickSlotType;
	UPROPERTY(EditAnywhere)
	FText KeyText;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> DefaultImage;
	UPROPERTY(Transient)
	TObjectPtr<class UInventoryData> InventoryData;


	//이 슬롯의 위치정보
	UPROPERTY(VisibleAnywhere)
	int32 Index = 0;
	//메인위젯에 붙어있는 퀵슬롯 위젯입니다.
	UPROPERTY(Transient)
	TWeakObjectPtr<class UQuickSlotWidget> MainQuickSlotWidget;


	//이 슬롯에 아이템이나 어빌리티가 등록되면 호출됩니다.
	FOnRegisterItemOrAbility OnRegisterItemOrAbility;
	//이 슬롯에 아이템이나 어빌리티가 등록 해제되면 호출됩니다.
	FOnRegisterItemOrAbility OnUnRegisterItemOrAbility;


	//메인위젯에 붙어있는 퀵슬롯과 연동합니다.
	void LinkToMainWidgetSlot(bool bClear = false);
public:
	void SetQuickSlotType(EQuickSlotType NewType);

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void Init(UInventoryData* Data);
	void Clean();

	//메인위젯에 붙어있는 퀵슬롯 버튼만 호출하세요. 무한루프를 방지합니다.
	void CleanMainQuickSlotButton();

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	
	//장비창에서 이미 등록된 아이템을 다른 슬롯에 등록하려 했을 때, 기존슬롯의 정보를 지우는 이벤트
	FOnRemoveAlreadyRegisteredSlot OnRemoveAlreadyRegisteredSlot;

	void SetIndex(int32 NewIndex);
	//또다른 퀵슬롯이 드롭되었을 때 처리하는 함수입니다.
	void OnDropOtherQuickSlot(class UQuickSlotButtonWidget* OtherQuickSlotButtonWidget);
	//인벤토리 버튼이 드롭되었고, 소비아이템인경우 처리합니다.
	void OnDropConsumeItem(class UItemButtonWidget* InventoryButtonWidget);
	//인벤토리 버튼이 드롭되었고, 어빌리티인경우 처리합니다.
	void OnDropAbility(class UItemButtonWidget* InventoryButtonWidget);
	//이미 퀵에 등록되어있는지 확인합니다.
	bool IsAlreadyRegistered(class UItemData* Data) const;
	bool IsAlreadyRegistered(class UAbilityData* Data) const;
	UFUNCTION()
	void UseQuickSlot(UInputAction* InputAction);
	//아이템 카운트 변경시 슬롯 정보를 업데이트 하는 이벤트 함수입니다.
	UFUNCTION()
	void OnRefreshItemSlot(const FGuid& ItemUniqueID, const int32& NewCount);
	//이 슬롯에 등록된 아이템 정보가 제거되었을 때 호출되는 이벤트 함수입니다.
	UFUNCTION()
	void OnSlotItemRemoved(ABaseCharacter* Player, const FGuid& ItemUniqueID);

	//어빌리티가 제거되었을 때, 이 슬롯에 등록된 어빌리티인지 확인하고 업데이트하는 이벤트 함수입니다.
	UFUNCTION()
	void OnRemoveAbilityEvent(const FGameplayTag& AbilityTag);

	FGuid GetSlotItemUniqueID();
	FGameplayTag GetSlotAbilityTag();
	int32 GetSlotIndex() const {return Index;}
	
	void RestoreSlotFromItemGUID(const FGuid& ItemID);
	void RestoreSlotFromAbilityTag(const FGameplayTag& AbilityTag);

	UInventoryData* GetQuickSlotData() const {return InventoryData.Get(); }
};
