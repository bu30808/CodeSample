// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Menu/05_Orb/OrbMatrixElementWidget.h"
#include "Blueprint/UserWidget.h"
#include "SimpleToolTipWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API USimpleToolTipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Description;

public:
	UFUNCTION(BlueprintCallable)
	void SetDescriptionText(FString Msg);
	UFUNCTION(BlueprintCallable)
	void AddDescriptionText(const FString& AddDescription);
};
