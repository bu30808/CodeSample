// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NavigationWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UNavigationWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class UNavigationButtonWidget* Button_CharacterInformation;
	UPROPERTY(meta=(BindWidget))
	class UNavigationButtonWidget* Button_Item;
	UPROPERTY(meta=(BindWidget))
	class UNavigationButtonWidget* Button_Equip;
	UPROPERTY(meta=(BindWidget))
	class UNavigationButtonWidget* Button_Orb;
	UPROPERTY(meta=(BindWidget))
	class UNavigationButtonWidget* Button_Option;

	virtual void NativeConstruct() override;


	UPROPERTY()
	TWeakObjectPtr<class UMainWidget> MainWidget;

public:
	void Init(class UMainWidget* Widget);
};
