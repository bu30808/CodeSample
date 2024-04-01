// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/09_Resource/ResourceWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UResourceWidget::NativePreConstruct()
{
	ParentImage = Image_Progress;
	Super::NativePreConstruct();
}

void UResourceWidget::UpdateProgressLength(float MaxValue)
{
	if (Image_Progress)
	{
		if (auto slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Progress))
		{
			auto y = slot->GetSize().Y;
			auto len = FMath::Clamp(MaxValue * 3.f, 0, 1920);
			UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(len));
			slot->SetSize(FVector2D(len, y));
		}
	}
}
