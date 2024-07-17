// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/04_NPC/99_Component/RegisterAbilityComponent.h"

#include "00_Character/04_NPC/NPCBase.h"
#include "03_Widget/07_NPC/00_Bonfire/RegisterAbilityWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "Components/Button.h"


// Sets default values for this component's properties
URegisterAbilityComponent::URegisterAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	static ConstructorHelpers::FClassFinder<URegisterAbilityWidget> widget(TEXT(
		"/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/02_Widget/NPC/RegisterAbility/UMG_RegisterAbility.UMG_RegisterAbility_C'"));
	if (widget.Succeeded())
	{
		RegisterAbilityWidgetClass = widget.Class;
	}
}


// Called when the game starts
void URegisterAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void URegisterAbilityComponent::AddRegisterAbilityWidget(APlayerCharacter* Player)
{
	if (RegisterAbilityWidget == nullptr)
	{
		if (Player)
		{
			if (RegisterAbilityWidgetClass)
			{
				if (const auto pc = Player->GetController<APlayerController>())
				{
					if (RegisterAbilityWidget.IsValid() == false)
					{
						RegisterAbilityWidget = CreateWidget<URegisterAbilityWidget>(pc,
							RegisterAbilityWidgetClass);
						RegisterAbilityWidget->Button_Close->OnClicked.AddUniqueDynamic(
							GetOwner<ANPCBase>(), &ANPCBase::FinishInteraction);
					}

				}
			}
		}
	}
	//UWidgetHelperLibrary::OpenWidgetSetting(GetOwner<ANPCBase>()->GetInteractionActor()->GetController<APlayerController>(),RegisterAbilityWidget.Get());
	
	if (RegisterAbilityWidget->IsInViewport() == false)
	{
		RegisterAbilityWidget->AddToViewport();
	}else{
		RegisterAbilityWidget->SetVisibility(ESlateVisibility::Visible);
		RegisterAbilityWidget->CreateRegisterAbilityList();
	}
}
