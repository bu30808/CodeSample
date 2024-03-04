// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbElementListWidget.h"
#include "03_Widget/99_Drag/DraggableWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "OrbListButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredInventoryOrb,class UOrbData*,Data);
/**
 * 
 */
UCLASS()
class SOULLIKE_API UOrbListButtonWidget : public UDraggableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image;
	/*UPROPERTY(meta=(BindWidget)) 
	class UTextBlock* TextBlock_Name;*/
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Equipped;

	UPROPERTY(EditAnywhere)
	USoundBase* HoverSound;
	
	UPROPERTY()
	TObjectPtr<class UOrbData> OrbData;

	virtual void NativeConstruct() override;

	UPROPERTY(Blueprintable,BlueprintCallable)
	FOnHoveredInventoryOrb OnHovered;
	UFUNCTION()
	void OnHoveredEvent();
public:
	class UOrbData* GetOrbData() const {return OrbData.Get();}
	const FInventoryItem& GetOrbItem() const { return OrbData->Data; }
	
	void SetData(UObject* Item);
	void SetEquipped(bool bEquipped);
	void UpdateData(const FInventoryItem& OrbInfo);
	

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
