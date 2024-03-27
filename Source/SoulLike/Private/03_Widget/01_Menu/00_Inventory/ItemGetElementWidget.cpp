// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/01_Menu/00_Inventory/ItemGetElementWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UItemGetElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//3초 후 제거
	if (GetOwningPlayer())
	{
		FTimerHandle removeTimerHandle;
		GetOwningPlayer()->GetWorldTimerManager().SetTimer(removeTimerHandle, this,
		                                                   &UItemGetElementWidget::RemoveFromParent, 3.f);

		UGameplayStatics::PlaySound2D(GetOwningPlayer(), PickUpSound);
	}
}

void UItemGetElementWidget::SetData(const TSoftObjectPtr<UTexture2D>& Image, const FText& Name, int32 Count) const
{
	Image_Item->SetBrushFromSoftTexture(Image);
	TextBlock_ItemName->SetText(Name);
	TextBlock_Count->SetText(FText::AsNumber(Count));
}
