// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NavigationButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UNavigationButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button;

	virtual void NativeConstruct() override;
};
