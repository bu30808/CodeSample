// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "ItemInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UItemInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Item;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemName;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemType;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemCount;
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_ItemInfo;
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_ItemDetail;

public:
	void SetInfo(const FInventoryItem& Item);
};
