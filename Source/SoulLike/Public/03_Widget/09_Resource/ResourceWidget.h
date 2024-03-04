// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "03_Widget/00_Gradient/GradientBaseWidget.h"
#include "Blueprint/UserWidget.h"
#include "ResourceWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UResourceWidget : public UGradientBaseWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget),BlueprintReadOnly)
	class UImage* Image_Progress;

	virtual void NativePreConstruct() override;
	class UMaterialInstanceDynamic* InitDynamicMaterial(class UMaterialInterface* ProgressMat,UImage* ProgressImage,FLinearColor Color) const;
public:
	void UpdateProgressLength(float MaxValue);
};
