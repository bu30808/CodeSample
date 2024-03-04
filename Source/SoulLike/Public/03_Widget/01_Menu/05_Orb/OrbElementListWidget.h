// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "04_Item/01_Equipment/03_Orb/EquipmentItemActor_OrbCore.h"
#include "Blueprint/UserWidget.h"
#include "OrbElementListWidget.generated.h"


enum class EOrbType : uint8;

//코어나 프래그먼트
UCLASS()
class UOrbData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FInventoryItem Data;
	UPROPERTY()
	bool bIsEquipped = false;
};

UENUM(BlueprintType)
enum class EOrbElementListType : uint8
{
	CORE,
	FRAGMENT
};

/**
 * 
 */
UCLASS()
class SOULLIKE_API UOrbElementListWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UOrbElementFilterWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_Filter;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_OpenFilter;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_InitFilter;

	UPROPERTY(meta=(BindWidget))
	class UWrapBox* WrapBox;
	UPROPERTY(meta=(BindWidget))
	class UWrapBox* WrapBox_Filter;
	
	/*
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* UniformGridPanel;
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* UniformGridPanel_Filter;
	UPROPERTY(EditAnywhere)
	int32 MaxRow;
	UPROPERTY(EditAnywhere)
	int32 MaxCol;
	
	UPROPERTY()
	int32 Row = 0;
	UPROPERTY()
	int32 Col = -1;
	*/

	UPROPERTY()
	TArray<class UOrbListButtonWidget*> WrappedChild;
	UPROPERTY()
	TArray<class UOrbListButtonWidget*> FilteredChild;
	
	UPROPERTY(EditAnywhere)
	EOrbElementListType OrbElementListType;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UOrbListButtonWidget> OrbListButtonWidgetObject;

	

	//필터기능에 의해 걸러진 아이템 리스트입니다.
	UPROPERTY()
	TArray<class UOrbData*> FilteredItemList;
	UPROPERTY()
	TArray<class UOrbElementFilterWidget*> FilterWidgets;

	TArray<EAttributeType> AttributeFilter;
	TSet<EOrbMatrixSlotType> SlotFilter;
	TSet<EOrbMatrixSlotType> SlotFilterFromElement;

	UPROPERTY()
	TWeakObjectPtr<UUserWidget> ParentsUMG;


	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	/*void OnEntryWidgetGeneratedEvent(UUserWidget& UserWidget);*/
public:
	UFUNCTION()
	void OnClickedOpenFilter();
	UFUNCTION()
	void OnClickedInitFilter();

	void InitFilterFromElement();

	/*void AddItem(const FOrbCoreInformation* Info);*/
	void AddItem(const FInventoryItem& OrbInfo);
	void SetEquipped(const FGuid& OrbID, bool bEquipped);
	
	//모든 필터를 재 적용 하도록 합니다.
	void ForceReFiltering();
	/*TArray<UOrbData*> FindSameSlotType();*/
	TArray<UOrbData*> FindSameSlotType(const TArray<UOrbData*>& FilteredByAttribute);
	/*TArray<UOrbData*> FindSameAttribute(const TArray<UOrbData*>& FilteredBySlotType);*/
	TArray<UOrbData*> FindSameAttribute(const TArray<UOrbData*>& Items, EAttributeType FindAttribute);
	bool IsAllFilterEmpty();

	void OnUpdateFilterEvent();

	void AddFilter(EOrbMatrixSlotType SlotType);
	void AddFilterFromElement(EOrbMatrixSlotType SlotType);
	void AddFilter(EAttributeType AttributeType);
	void RemoveFilter(EOrbMatrixSlotType SlotType);
	void RemoveFilter(EAttributeType AttributeType);
	void RemoveFilterFromElement(EOrbMatrixSlotType SlotType);
	void CreateCoreListFromInventory();
	void CreateFragmentListFromInventory();
	void RefreshFromInventoryItem(const FInventoryItem& Item);
	void Refresh();
	void EnhancementSetting(UUserWidget* ParentsWidget);
	UFUNCTION()
	void OnItemButtonHovered(class UOrbData* Data);

	void SortElement();

	void GetWidgetBySlotType(EOrbMatrixSlotType Type);

	virtual void RemoveFromParent() override;
	
};
