// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/00_Player/00_Controller/00_Component/WidgetManagerComponent.h"

#include "00_Character/00_Player/00_Controller/UserController.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UWidgetManagerComponent::UWidgetManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWidgetManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

UClass* UWidgetManagerComponent::GetWidgetClassFromTable(FGameplayTag WidgetTag)
{
	if (WidgetTable)
	{
		FString contextString;
		if (auto data = WidgetTable->FindRow<FWidgetTable>(WidgetTag.GetTagName(), contextString))
		{
			if (auto widgetClass = data->WidgetSoftClass.LoadSynchronous())
			{
				return widgetClass;
			}
		}
	}

	return nullptr;
}

UUserWidget* UWidgetManagerComponent::AddWidget(FGameplayTag WidgetTag, int32 ZOrder, bool bSave, bool bAddToViewport)
{
	//위젯을 저장하지 않고 생성하는 경우
	if (bSave == false)
	{
		if (auto widgetClass = GetWidgetClassFromTable(WidgetTag))
		{
			if (auto widget = CreateWidget<UUserWidget>(GetOwner<AUserController>(), widgetClass))
			{
				if(bAddToViewport)
				{
					widget->AddToViewport(ZOrder);
				}
				return widget;
			}
		}
	}

	if (!Widgets.Contains(WidgetTag))
	{
		if (auto widgetClass = GetWidgetClassFromTable(WidgetTag))
		{
			if (auto widget = CreateWidget<UUserWidget>(GetOwner<AUserController>(), widgetClass))
			{
				Widgets.Add(WidgetTag, widget);
				if(bAddToViewport)
				{
					widget->AddToViewport(ZOrder);
				}
				return widget;
			}
		}
	}
	else
	{
		if (Widgets[WidgetTag]->IsValidLowLevel() == false)
		{
			if (auto widgetClass = GetWidgetClassFromTable(WidgetTag))
			{
				if (auto widget = CreateWidget<UUserWidget>(GetOwner<AUserController>(), widgetClass))
				{
					Widgets[WidgetTag] = widget;
					if(bAddToViewport)
					{
						widget->AddToViewport(ZOrder);
					}
					return widget;
				}
			}
		}

		if (Widgets[WidgetTag]->IsInViewport() == false)
		{
			if(bAddToViewport)
			{
				Widgets[WidgetTag]->AddToViewport(ZOrder);
			}
		}

		return Widgets[WidgetTag];
	}

	return nullptr;
}

UUserWidget* UWidgetManagerComponent::GetWidget(FGameplayTag WidgetTag)
{
	if (Widgets.Contains(WidgetTag))
	{
		return Widgets[WidgetTag];
	}

	return nullptr;
}

void UWidgetManagerComponent::RemoveWidget(FGameplayTag WidgetTag)
{
	if (Widgets.Contains(WidgetTag))
	{
		auto widget = Widgets[WidgetTag];
		Widgets.Remove(WidgetTag);
		widget->RemoveFromParent();
	}
}
