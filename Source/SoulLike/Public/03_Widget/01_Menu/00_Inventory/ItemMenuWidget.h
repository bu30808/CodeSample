// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ItemMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UItemMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Use;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Remove;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Cancel;

public:
	UPROPERTY()
	TWeakObjectPtr<class UUserWidget> ParentsWidget;

	FOnButtonClickedEvent OnClickedUseButton;
	FOnButtonClickedEvent OnClickedRemoveButton;
	FOnButtonClickedEvent OnClickedCancelButton;

	UFUNCTION()
	void OnClickedUseButtonEvent();
	UFUNCTION()
	void OnClickedRemoveButtonEvent();
	UFUNCTION()
	void OnClickedCancelButtonEvent();

	virtual void NativeConstruct() override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
