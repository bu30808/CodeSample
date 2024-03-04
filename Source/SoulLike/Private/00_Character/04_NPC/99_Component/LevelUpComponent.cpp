// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/99_Component/LevelUpComponent.h"

#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/03_LevelUp/LevelUpWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Logging/StructuredLog.h"

// Sets default values for this component's properties
ULevelUpComponent::ULevelUpComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	static ConstructorHelpers::FClassFinder<ULevelUpWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/NPC/LevelUp/UMG_LevelUp.UMG_LevelUp_C'"));
	if (widget.Succeeded())
	{
		LevelUpWidgetObject = widget.Class;
	}
}

void ULevelUpComponent::OnLevelUpWidgetVisibilityChangedEvent(ESlateVisibility InVisibility)
{
	if(LevelUpWidget.IsValid())
	{
		if(!LevelUpWidget->IsVisible())
		{
			LevelUpWidget = nullptr;
		}
	}
}

void ULevelUpComponent::CreateLevelUpWidget()
{
	if(LevelUpWidget==nullptr)
	{
		if (LevelUpWidgetObject)
		{
			if (auto owner = GetOwner<ANPCBase>())
			{
				auto pc = owner->GetInteractionActor()->GetController<APlayerController>();

				if (LevelUpWidget.IsValid() == false)
				{
					LevelUpWidget = CreateWidget<ULevelUpWidget>(pc, LevelUpWidgetObject);
					LevelUpWidget->Button_Close->OnClicked.AddUniqueDynamic(
						GetOwner<ANPCBase>(), &ANPCBase::FinishInteraction);
				}
			
				LevelUpWidget->OnVisibilityChanged.AddUniqueDynamic(this,&ULevelUpComponent::OnLevelUpWidgetVisibilityChangedEvent);
			}
		}
	}
}


void ULevelUpComponent::ShowLevelUpWidget()
{
	CreateLevelUpWidget();
	
	if (LevelUpWidget->IsInViewport() == false)
	{
		LevelUpWidget->AddToViewport();
	}
}
