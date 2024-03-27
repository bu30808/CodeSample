// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "ItemGetElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UItemGetElementWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Item;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemName;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Count;

	UPROPERTY(EditAnywhere)
	class USoundBase* PickUpSound;

public:
	virtual void NativeConstruct() override;
	void SetData(const TSoftObjectPtr<UTexture2D>& Image, const FText& Name, int32 Count) const;
};
