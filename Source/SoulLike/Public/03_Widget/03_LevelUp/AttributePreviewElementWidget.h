// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "02_Ability/AbilityEffect.h"
#include "Blueprint/UserWidget.h"
#include "AttributePreviewElementWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UAttributePreviewElementWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* RichTextBlock_Icon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Cur;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextBlock_Next;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_Next;

	UPROPERTY(EditAnywhere)
	EAttributeType AttributeType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USimpleToolTipWidget> ToolTipWidgetObject;

	//지금 이 위젯이 표시하고있는 기본값.
	float BaseValue;
	//다른 스텟 등의 영향으로 누적해서 증가한 값
	float AccValue;

public:
	EAttributeType GetAttributeType() { return AttributeType; }
	virtual void NativeConstruct() override;
	void Init(EAttributeType Attribute);

	/*void SetPreview(float PreviewVal) const;*/
	void Reset() const;

	void UpdatePreview(float AddValue);
};
