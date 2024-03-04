// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget\08_StatusEffect\StatusEffectProgressWidget.h"

#include "Components/Image.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UStatusEffectProgressWidget::SetDynamicMaterial()
{
	if(Image_Progress)
	{
		if (ProgressMaterial)
		{
			DynamicMat = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), ProgressMaterial);
			if (DynamicMat)
			{
				DynamicMat->SetVectorParameterValue("ProgressColor1", DefaultProgressColor);
				if (Image_Progress != nullptr)
				{
					Image_Progress->SetBrushFromMaterial(DynamicMat);
				}
			}
		}
	}
}

void UStatusEffectProgressWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetDynamicMaterial();
	DynamicMat->SetVectorParameterValue("ProgressColor1",DefaultProgressColor);
}

void UStatusEffectProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetDynamicMaterial();
}

void UStatusEffectProgressWidget::SetProgress(float Value)
{
	if(DynamicMat)
	{
	//	UKismetSystemLibrary::PrintString(this,FString::SanitizeFloat(Value));
		DynamicMat->SetScalarParameterValue("Progress",Value);

		//가득 찼다면,
		if(Value >= 1.f)
		{
			//프로그래스바의 색을 변경합니다.
			DynamicMat->SetVectorParameterValue("ProgressColor1",ProgressFullColor);
		}
	}
}

void UStatusEffectProgressWidget::SetImage(EStatusEffect StatusEffect)
{
	if(StatusEffectImage.Contains(StatusEffect))
	{
		Image_StatusEffect->SetBrushFromSoftTexture(StatusEffectImage[StatusEffect]);
	}
}
