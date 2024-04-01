// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/07_NPC/00_Bonfire/BonfireTeleportWidget.h"

#include "00_Character/04_NPC/Bonfire.h"
#include "00_Character/04_NPC/99_Component/BonfireComponent.h"
#include "03_Widget/07_NPC/00_Bonfire/TeleportElementWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "98_GameInstance/SoulLikeInstance.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"

void UBonfireTeleportWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;
	Super::NativeConstruct();

	Button_OK->OnClicked.AddUniqueDynamic(this, &UBonfireTeleportWidget::OnClickedOKEvent);
	Button_Cancel->OnClicked.AddUniqueDynamic(this, &UBonfireTeleportWidget::OnClickedCancelEvent);
}

void UBonfireTeleportWidget::OnClickedOKEvent()
{
	if (SelectedElement.IsValid())
	{
		const auto& info = SelectedElement->GetTeleportInformation();

		//이 정보를 저장한 뒤, 레벨을 다시 로드합니다.
		if (auto instance = Cast<USoulLikeInstance>(GetGameInstance()))
		{
			//다음에 로드시 이동해야할 화톳불 정보를 저장합니다.
			//다시 로드할 때, 위 정보에 해당하는 위치로 이동하도록 합니다.
			SetVisibility(ESlateVisibility::Collapsed);
			instance->SaveTeleportBonfire(info);
			GetOwningPlayerPawn<APlayerCharacter>()->TeleportToOtherBonfire(info);
			//UGameplayStatics::OpenLevel(GetWorld(),FName(info.LevelName));
		}
	}
}

void UBonfireTeleportWidget::OnClickedCancelEvent()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UBonfireTeleportWidget::CreateList(UBonfireComponent* BonfireComponent)
{
	if (BonfireComponent && TeleportElementWidgetObject != nullptr)
	{
		ScrollBox_TeleportList->ClearChildren();

		auto owner = Cast<ABonfire>(BonfireComponent->GetOwner());
		const auto& list = owner->GetTeleportList();
		for (auto iter : list)
		{
			
			if (auto element = CreateWidget<UTeleportElementWidget>(GetOwningPlayer(), TeleportElementWidgetObject))
			{
				element->SetInfo(*iter, this);

				//UKismetSystemLibrary::PrintString(this, iter->LocationName + "/" + owner->GetBonfireInformation().LocationName);
				ScrollBox_TeleportList->AddChild(element);

				if (iter->LocationName.EqualTo(owner->GetBonfireInformation().LocationName))
				{
					SetPreviewImage(iter->LocationImage);
					element->ShowCurLocation();
				}

				element->SetIsEnabled(IsActivated(UGameplayStatics::GetCurrentLevelName(BonfireComponent),
				                                  iter->OwnersSafeName));
			}
		}
	}
}

bool UBonfireTeleportWidget::IsActivated(const FString& LevelName, const FString& SafeName)
{
	//이 화톳불이 활성화 되었다고 저장합니다.
	if (auto instance = Cast<USoulLikeInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		return instance->IsActivatedBonfire(LevelName, SafeName);
	}
	return false;
}

void UBonfireTeleportWidget::SetPreviewImage(const TSoftObjectPtr<UTexture2D>& LocationImage)
{
	Image_Location->SetBrushFromSoftTexture(LocationImage);
}

void UBonfireTeleportWidget::SetSelectedInformation(UTeleportElementWidget* TeleportElementWidget)
{
	if (SelectedElement.IsValid())
	{
		SelectedElement->SetUnSelected();
	}

	SelectedElement = TeleportElementWidget;
	TeleportElementWidget->SetSelected();
}
