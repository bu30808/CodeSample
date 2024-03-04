// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/00_Gradient/GradientBaseWidget.h"

#include "Components/Image.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Logging/StructuredLog.h"

void UGradientBaseWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetDynamicMaterial();
}

void UGradientBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGradientBaseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (DynamicMat && ParentImage)
	{
		CurAlpha = CurAlpha + (InDeltaTime / ResponseTime);

		float Alpha = FMath::Clamp(CurAlpha, 0, 1);
		DynamicMat->SetScalarParameterValue(ProgressParamName, FMath::Lerp(PrePercent, Percent, Alpha));
	}
}

void UGradientBaseWidget::SetDynamicMaterial()
{
	if (GradientBarMaterial)
	{
		DynamicMat = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), GradientBarMaterial);
		if (DynamicMat)
		{
			DynamicMat->SetVectorParameterValue("Color", Color);
			if (ParentImage != nullptr)
			{
				ParentImage->SetBrushFromMaterial(DynamicMat);
				//DynamicMat->SetScalarParameterValue(ProgressParamName, 0.5f);
			}
		}
	}
}

void UGradientBaseWidget::UpdateProgress(float cur, float max)
{
	PrePercent = Percent;
	Percent = FMath::Clamp(cur / max, 0.f, 1.f);
	CurAlpha = 0;
}
