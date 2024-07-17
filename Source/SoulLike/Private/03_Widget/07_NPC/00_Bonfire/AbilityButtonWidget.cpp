// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/07_NPC/00_Bonfire/AbilityButtonWidget.h"

#include "02_Ability/AbilityRequirement.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Image.h"
#include "SoulLike/SoulLike.h"


void UAbilityButtonWidget::SetInfo(const FAbilityInformation& Info)
{
	AbilityInformation = Info;
	Image->SetBrushFromSoftTexture(Info.AbilityImage);
	if(AbilityInformation.AbilityTag.IsValid()){
		UWidgetHelperLibrary::SetToolTipWidget(this,FText::Format(FText::FromString("{0}\n\n{1}"),AbilityInformation.AbilityDescription,AbilityInformation.GetAbilityReqDescription()));
	}
}

void UAbilityButtonWidget::SetEquipped(bool bCond)
{
	UE_LOGFMT(LogUMG,Log,"{0} 장착 여부 설정 : {1}",GetName(),bCond);
	if(bCond)
	{
		Image_Equipped->SetVisibility(ESlateVisibility::Visible);
	}else
	{
		Image_Equipped->SetVisibility(ESlateVisibility::Collapsed);
	}
}



