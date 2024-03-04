// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/02_Monster/HealthBarWidget.h"

#include "Logging/StructuredLog.h"

void UHealthBarWidget::NativePreConstruct()
{
	if (Image_Progress)
	{
		ParentImage = Image_Progress;
		Super::NativePreConstruct();
	}
}

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//UpdateProgress(1,1);
}
