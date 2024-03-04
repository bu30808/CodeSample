// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/00_Gradient/CircleBaseWidget.h"

#include "Components/TextBlock.h"

void UCircleBaseWidget::NativePreConstruct()
{
	ParentImage = Image_Progress;

	ProgressParamName = "percentage";
	Super::NativePreConstruct();
}

void UCircleBaseWidget::UpdateProgress(float cur, float max)
{
	Super::UpdateProgress(cur, max);

	FString percentText = FString::FormatAsNumber(cur) + "/" + FString::FormatAsNumber(max);
	TextBlock_Percent->SetText(FText::FromString(percentText));
}
