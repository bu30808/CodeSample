// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "EnhancementWidget.generated.h"

class UOrbData;
enum class EAttributeType : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UEnhancementWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()
	friend class UEnhancementComponent;
	friend class UEnhancementButtonWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_Materials;
	UPROPERTY(meta=(BindWidget))
	class UWrapBox* WrapBox_Attribute;

	UPROPERTY(meta=(BindWidget))
	class UEnhancementButtonWidget* UMG_EnhancementButton;

	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Cur;
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Next;
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Material;
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_NeedSoul;

	UPROPERTY(meta=(BindWidget))
	class UBorder* Border_UpgradeMax;
	
	UPROPERTY(meta=(BindWidget))
	class UItemInfoWidget* UMG_ItemInfo;

	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
	UPROPERTY(meta=(BindWidget))
	class UOrbElementListWidget* UMG_OrbElementListCore;
	UPROPERTY(meta=(BindWidget))
	class UOrbElementListWidget* UMG_OrbElementListFragment;
	UPROPERTY(meta=(BindWidget))
	class UItemListWidget* UMG_Inventory_Spirit;
	UPROPERTY(meta=(BindWidget))
	class UItemListWidget* UMG_Inventory_Armor;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Core;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Fragment;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Spirit;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Armor;


	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Upgrade;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEnhancementAttributeWidget> EnhancementAttributeObject;
	/*UPROPERTY(EditAnywhere)
	TArray<EAttributeType> AllowEnhancementAttributes;*/
	UPROPERTY(EditAnywhere)
	class USoundBase* OpenSound;

	UPROPERTY()
	TArray<class UEnhancementAttributeWidget*> CheckBoxes;
	UPROPERTY()
	TWeakObjectPtr<class UEnhancementComponent> EnhancementComponent;

	//이 위젯에 정보가 설정된 아이템구조체입니다.
	UPROPERTY()
	FInventoryItem SetItem;
	


	virtual void NativePreConstruct() override;

	UFUNCTION()
	void OnClickedArmor();
	UFUNCTION()
	void OnClickedSpirit();
	UFUNCTION()
	void OnClickedFragment();
	UFUNCTION()
	void OnClickedCore();
	UFUNCTION()
	void OnClickedUpgrade();
	UFUNCTION()
	void OnClickedCloseButton();

	void CreateEnhancementAttributes(const FInventoryItem& Item);
	void RefreshItemList(const FInventoryItem& Item);
	virtual void NativeConstruct() override;

public:
	void ForceUnCheckOthers(class UEnhancementAttributeWidget* CheckedWidget);
	void SetEnhancementComponent(class UEnhancementComponent* E_Component);
	//강화에 필요한 정보를 설정합니다.
	void SetEnhancementInfo(const FInventoryItem& Item);
	//아이템 정보 표시
	void ShowItemInformation(UInventoryData* Data);
	//오브 및 프레그먼트 정보 표시
	void ShowItemInformation(UOrbData* Data);
	
	UFUNCTION()
	void OnUpgradeEquipmentEvent(const FGuid& ID, AEquipmentItemActor* EquipmentItemActor);
};
