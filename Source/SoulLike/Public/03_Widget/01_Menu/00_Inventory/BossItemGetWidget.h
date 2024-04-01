// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossItemGetWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UBossItemGetWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBossItemGetElementWidget> BossItemGetElementObject;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_Element;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_OK;

	UFUNCTION()
	void OnClicked();

	virtual void NativeConstruct() override;

public:
	void AddItemElement(class AItemActor* Item);
};
