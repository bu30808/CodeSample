// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemListWidget.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UInventoryWidget : public UPopUpBasedWidget
{
	GENERATED_BODY()

	friend class UGameLoadHandler;

protected:
	UPROPERTY(meta=(BindWidget))
	class UItemListWidget* UMG_ItemList;
	UPROPERTY(meta=(BindWidget))
	class UItemInfoWidget* UMG_ItemInfo;
	UPROPERTY(meta=(BindWidget))
	class UCharacterStatusWidget* UMG_CharacterInfo;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Close;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnClickedCloseButton();

public:
	void ShowItemInformation(class UInventoryData* Data);
};
