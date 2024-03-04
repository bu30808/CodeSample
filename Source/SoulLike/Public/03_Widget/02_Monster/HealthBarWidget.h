// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "03_Widget/00_Gradient/GradientBaseWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UHealthBarWidget : public UGradientBaseWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UImage* Image_Progress;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	//virtual void UpdateProgress(float cur, float max) override;
};
