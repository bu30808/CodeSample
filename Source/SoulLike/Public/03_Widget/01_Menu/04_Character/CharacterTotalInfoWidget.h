// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "03_Widget/97_PopUp/PopUpBasedWidget.h"
#include "Blueprint/UserWidget.h"
#include "CharacterTotalInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UCharacterTotalInfoWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

	friend class UMainWidget;
	
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UCharacterRenderWidget* UMG_CharacterRender;
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_EquipInfo;
	UPROPERTY()
	TArray<class UWidget*> EquipmentButtons;
	UPROPERTY()
	TWeakObjectPtr<class UEquipWidget> EquipmentWidget;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnEquippedItemEvent(class ABaseCharacter* UsedBy, const FInventoryItem& ItemInfo);

	UFUNCTION(BlueprintCallable)
	void AddRingSlot();
	UFUNCTION(BlueprintCallable)
	void RemoveRingSlot();
	
	virtual void OnVisibilityChangedEvent(ESlateVisibility InVisibility) override;
	
};
