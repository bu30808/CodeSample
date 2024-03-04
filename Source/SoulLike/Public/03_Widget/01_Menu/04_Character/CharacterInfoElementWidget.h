// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "Blueprint/UserWidget.h"
#include "02_Ability/AbilityEffect.h"
#include "CharacterInfoElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UCharacterInfoElementWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Text;

	UPROPERTY(EditAnywhere)
	EAttributeType AttributeType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USimpleToolTipWidget> SimpleToolTipWidgetObject;

public:
	void Init(EAttributeType NewType);
	//어트리뷰트 값을 다시 읽어와 업데이트 합니다.
	void UpdateAttributeValue();

protected:
	virtual void NativeConstruct() override;

	FString CreateImageString() const;
	FString MakeStringValue(const FAttribute& Attribute) const;
	FString GetAttributeValueString() const;
};
