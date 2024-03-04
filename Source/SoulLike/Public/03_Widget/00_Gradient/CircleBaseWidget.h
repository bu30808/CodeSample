// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/00_Gradient/GradientBaseWidget.h"
#include "CircleBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UCircleBaseWidget : public UGradientBaseWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Progress;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Percent;

	virtual void NativePreConstruct() override;

public:
	virtual void UpdateProgress(float cur, float max) override;
};
