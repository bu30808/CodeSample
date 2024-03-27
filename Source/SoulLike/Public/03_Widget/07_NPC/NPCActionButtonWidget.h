// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCActionButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UNPCActionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UNPCWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_ActionName;

public:
	void SetActionName(const FText& NewAction);
};
