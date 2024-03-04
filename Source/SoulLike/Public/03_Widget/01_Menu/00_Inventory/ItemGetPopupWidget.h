// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "ItemGetPopupWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UItemGetPopupWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UItemGetElementWidget> ItemGetElementObject;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnGetItem(TSoftObjectPtr<UTexture2D> Image, FString Name, int32 Count);
};
