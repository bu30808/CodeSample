// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/02_Monster/HealthBarWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Logging/StructuredLog.h"


void UHealthBarWidget::SetDynamicMaterial()
{
	if (GradientBarMaterial)
	{
		DynamicMat = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), GradientBarMaterial);
		if (DynamicMat)
		{
			DynamicMat->SetVectorParameterValue("ProgressColor1", Color);
			if (ParentImage != nullptr)
			{
				ParentImage->SetBrushFromMaterial(DynamicMat);
				//DynamicMat->SetScalarParameterValue(ProgressParamName, 0.5f);
			}
		}
	}
}


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
	TextBlock_Damage->SetVisibility(ESlateVisibility::Hidden);
	AccDamage = 0;
	//UpdateProgress(1,1);
}

void UHealthBarWidget::HideDamage()
{
	TextBlock_Damage->SetVisibility(ESlateVisibility::Hidden);
	AccDamage = 0;
}

void UHealthBarWidget::ShowDamage(float Damage)
{
	if(Damage>=0)
	{
		AccDamage+=Damage;
		UE_LOGFMT(LogTemp,Log,"표시할 피해량 : {0}", Damage);
		
		//피해량을 표시합니다.
		TextBlock_Damage->SetVisibility(ESlateVisibility::Visible);
		TextBlock_Damage->SetText(FText::AsNumber(AccDamage));
		//n초후에 제거합니다.
		if(GetOwningPlayer()->GetWorldTimerManager().TimerExists(visibilityTimerHandle))
		{
			GetOwningPlayer()->GetWorldTimerManager().ClearTimer(visibilityTimerHandle);
		}
			
		GetOwningPlayer()->GetWorldTimerManager().SetTimer(visibilityTimerHandle,this,&UHealthBarWidget::HideDamage,3.f,false);
	}
}

