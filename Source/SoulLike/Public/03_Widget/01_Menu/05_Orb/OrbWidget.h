// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbListButtonWidget.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "04_Item/ItemActor.h"
#include "Blueprint/UserWidget.h"
#include "OrbWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipCore, const FInventoryItem&, OrbCoreItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipFragment, const FInventoryItem&, OrbFragmentItem);

/**
 * 
 */
UCLASS()
class SOULLIKE_API UOrbWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

	friend class UOrbMatrixElementWidget;
	
public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_OrbRender;
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* UniformGridPanel_Matrix;
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
	UPROPERTY(meta=(BindWidget))
	class UOrbElementListWidget* UMG_OrbElementListCore;
	UPROPERTY(meta=(BindWidget))
	class UOrbElementListWidget* UMG_OrbElementListFragment;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Core;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Fragment;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UOrbMatrixElementWidget> OrbMatrixElementWidgetObject;

	//코어가 할당된 위젯 변수
	UPROPERTY()
	TWeakObjectPtr<class UOrbMatrixElementWidget> CoreMatrixElementWidget;
	//생성된 버튼을 저장하는 배열
	UPROPERTY()
	TArray<class UOrbMatrixElementWidget*> MatrixElements;

	UFUNCTION()
	void OnAddOrbItem(class ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo, AItemActor* ItemActor);



	void EquipOrbCore(const FInventoryItem& OrbCoreItem);
	void EquipOrbFragment(const FInventoryItem& OrbFragmentItem);

	void CreateOrbMatrix(const FInventoryItem& OrbCoreItem);

public:
	UFUNCTION()
	void OnClickedCoreButton();
	UFUNCTION()
	void OnClickedFragmentButton();
	UFUNCTION()
	void OnEquipCoreEvent(const FInventoryItem& OrbCoreItem);
	/*UFUNCTION()
	void OnEquipFragmentEvent(const FInventoryItem& OrbFragmentItem);*/


protected:
	virtual void CreateOrbMatrixFirst(const FInventoryItem& OrbCoreItem);
	virtual void OverrideOrbMatrix(const FInventoryItem& OrbCoreItem);

	void RefreshMatrix(const FInventoryItem& Core);

public:
	UOrbMatrixElementWidget* IsOrbEquipped(const FGuid& OrbID);
	
	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void SetEquippedInOrbListWidget(const FGuid& OrbID, EOrbType OrbType, bool bEquipped);
};
