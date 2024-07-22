// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/BuffIconWidget.h"

#include "Components/Image.h"

void UBuffIconWidget::SetIconInfo(const FAbilityInformation& AbilityInformation)
{
	Image_Buff->SetBrushFromSoftTexture(AbilityInformation.AbilityImage);
	BuffTag = AbilityInformation.AbilityTag;
}

bool UBuffIconWidget::IsTagMatch(const FGameplayTag& Tag) const
{
	return Tag.MatchesTagExact(BuffTag);
}
