// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnhancementAttributeWidget.generated.h"

enum class EAttributeType : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UEnhancementAttributeWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UEnhancementWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Attribute;
	UPROPERTY(meta=(BindWidget))
	class UImage* Image;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox;


	UPROPERTY()
	EAttributeType SelectedAttribute;
	UPROPERTY()
	TWeakObjectPtr<class UEnhancementWidget> EnhancementWidget;

	UPROPERTY(EditAnywhere)
	TMap<EAttributeType, TSoftObjectPtr<class UTexture2D>> Images;
	UPROPERTY(EditAnywhere)
	TMap<EAttributeType, FText> Texts;

	UFUNCTION()
	void OnChecked(bool bIsChecked);
	void SetInfo();

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
	void SetAttributeType(EAttributeType NewAttribute);
	void SetParent(class UEnhancementWidget* PWidget);
};
