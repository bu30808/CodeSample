// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/ItemActor.h"
#include "Blueprint/UserWidget.h"
#include "BossItemGetElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBossItemGetElementWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Item;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemName;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ItemCount;

public:
	void SetItemInformation(const FItemInformation* ItemInformation, const int32& ItemCount);
};
