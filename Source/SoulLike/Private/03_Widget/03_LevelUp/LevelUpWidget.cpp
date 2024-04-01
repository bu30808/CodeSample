// Fill out your copyright notice in the Description page of Project Settings.


#include "03_Widget/03_LevelUp/LevelUpWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "03_Widget/SimpleToolTipWidget.h"
#include "03_Widget/03_LevelUp/AttributePreviewWidget.h"
#include "03_Widget/03_LevelUp/LevelUpPreviewWidget.h"
#include "96_Library/WidgetHelperLibrary.h"
#include "99_Subsystem/LevelUpSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"

void ULevelUpWidget::SetUp()
{
	if (LevelUpSubsystem == nullptr)
	{
		if (auto pc = GetOwningPlayer())
		{
			if (auto local = pc->GetLocalPlayer())
			{
				LevelUpSubsystem = local->GetSubsystem<ULevelUpSubsystem>();
			}
		}
	}

	if (auto pawn = GetOwningPlayerPawn<APlayerCharacter>())
	{
		/*pawn->GetAttributeComponent()->OnCharacterInformationUpdate.AddUniqueDynamic(
			this, &ULevelUpWidget::Init);*/
	}

	if (Button_Close)
	{
		Button_Close->OnClicked.AddUniqueDynamic(this, &ULevelUpWidget::OnClickedCloseButton);
	}
	if (Button_Cancel)
	{
		Button_Cancel->OnClicked.AddUniqueDynamic(this, &ULevelUpWidget::OnClickedCancelButton);
	}
	if (Button_OK)
	{
		Button_OK->OnClicked.AddUniqueDynamic(this, &ULevelUpWidget::OnClickedOKButton);
	}
	if (WidgetTree)
	{
		TArray<UWidget*> allWidgets;
		WidgetTree->GetAllWidgets(allWidgets);

		for (auto w : allWidgets)
		{
			if (w->IsA<UAttributePreviewWidget>())
			{
				if (auto widget = Cast<UAttributePreviewWidget>(w))
				{
					AttributePreviewWidgets.Add(widget->AttributePointType, widget);
				}
			}
		}


		if (UMG_LevelUpPreview)
		{
			for (auto iter : AttributePreviewWidgets)
			{
				if (iter.Value)
				{
					iter.Value->Button_Add->OnClicked.AddUniqueDynamic(
						this, &ULevelUpWidget::OnClickedAttributeAddButton);

					iter.Value->OnAddAttributePreviewPoint.AddUObject(UMG_LevelUpPreview,
					                                                  &ULevelUpPreviewWidget::OnAddAttributePreviewPointEvent);


					/*iter.Value->OnAddAttributePreviewPoint.AddUniqueDynamic(UMG_LevelUpPreview,
					                                                        &ULevelUpPreviewWidget::
					                                                        OnAddAttributePreviewPointEvent);*/
				}
			}
		}
	}
	if (CanLevelUp())
	{
		for (auto iter : AttributePreviewWidgets)
		{
			iter.Value->ReadyToLevelUp();
		}
	}
}

void ULevelUpWidget::NativeConstruct()
{
	bUseAsPopUp = true;
	bRemovable = true;
	Super::NativeConstruct();

	SetUp();

	/*OnVisibilityChanged.AddUniqueDynamic(this, &ULevelUpWidget::OnVisibilityChangedEvent);
	UWidgetHelperLibrary::PopUpWidgetProcess(this, true);*/
}

void ULevelUpWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	//SetUp();
}

void ULevelUpWidget::Reset()
{
	Level_WantToUP = 0;

	UMG_LevelUpPreview->Reset();

	for (auto iter : AttributePreviewWidgets)
	{
		iter.Value->Reset();
	}

	if (CanLevelUp())
	{
		for (auto iter : AttributePreviewWidgets)
		{
			iter.Value->ReadyToLevelUp();
		}
	}
}

bool ULevelUpWidget::CanLevelUp() const
{
	if (auto pc = GetOwningPlayer())
	{
		if (auto local = pc->GetLocalPlayer())
		{
			if (const auto system = local->GetSubsystem<ULevelUpSubsystem>())
			{
				return system->CanLevelUp(GetOwningPlayerPawn<ABaseCharacter>()->GetAttributeComponent());
			}
		}
	}

	return false;
}

void ULevelUpWidget::Init()
{
	UMG_LevelUpPreview->Init();
	NextExpAcc = 0;
	for (auto iter : AttributePreviewWidgets)
	{
		iter.Value->Init();
	}
}

void ULevelUpWidget::OnClickedCloseButton()
{
	OnClosed.Broadcast();
	/*UWidgetHelperLibrary::CloseWidgetSetting(GetOwningPlayer(), this);
	RemoveFromParent();*/

	SetVisibility(ESlateVisibility::Collapsed);
}

void ULevelUpWidget::OnClickedCancelButton()
{
	Reset();
}

void ULevelUpWidget::OnClickedOKButton()
{
	if (LevelUpSubsystem.IsValid())
	{
		//어느 능력치를 얼마나 투자했는지 맵을 만듭니다.
		TMap<EAttributePointType, int32> AddAttributes;
		for (auto iter : AttributePreviewWidgets)
		{
			if (iter.Key != EAttributePointType::EXP && iter.Key != EAttributePointType::NEXT_EXP)
			{
				if (iter.Value->GetAddPoint() > 0)
				{
					AddAttributes.Add(iter.Key, iter.Value->GetAddPoint());

					UE_LOGFMT(LogTemp, Log, "더하려는 스텟 종류 : {0}",
					          StaticEnum<EAttributeType>()->GetValueAsString(iter.Key));

					iter.Value->AddPoint = 0;
				}
			}
		}

		const auto attComp = GetOwningPlayerPawn<APlayerCharacter>()->GetAttributeComponent();
		const auto curLevel = attComp->GetLevel();

		if (curLevel < curLevel + Level_WantToUP)
		{
			LevelUpSubsystem->TryLevelUp(attComp, curLevel + Level_WantToUP, AddAttributes);

			for (auto iter : AttributePreviewWidgets)
			{
				iter.Value->HorizontalBox_Next->SetVisibility(ESlateVisibility::Hidden);
			}
			UMG_LevelUpPreview->Init();
			Init();
		}
	}

	Level_WantToUP = 0;
}

void ULevelUpWidget::OnClickedAttributeAddButton()
{
	if (LevelUpSubsystem.IsValid())
	{
		auto attComp = GetOwningPlayerPawn<APlayerCharacter>()->GetAttributeComponent();
		auto curLevel = attComp->GetLevel();
		//다음 레벨로 가려는 경험치가 충분한지 확인합니다.
		if (LevelUpSubsystem->CanLevelUp(attComp, curLevel, curLevel + Level_WantToUP))
		{
			//올릴 레벨의 누적량을 늘립니다.
			Level_WantToUP++;

			//레벨 증가치 표시
			if (AttributePreviewWidgets.Contains(EAttributePointType::LEVEL))
			{
				AttributePreviewWidgets[EAttributePointType::LEVEL]->HorizontalBox_Next->SetVisibility(
					ESlateVisibility::SelfHitTestInvisible);
				AttributePreviewWidgets[EAttributePointType::LEVEL]->TextBlock_Next->SetText(
					FText::AsNumber(curLevel + Level_WantToUP));
				AttributePreviewWidgets[EAttributePointType::LEVEL]->AddPoint = Level_WantToUP;
			}

			//다음 경험치 필요량 표시
			if (AttributePreviewWidgets.Contains(EAttributePointType::NEXT_EXP))
			{
				AttributePreviewWidgets[EAttributePointType::NEXT_EXP]->HorizontalBox_Next->SetVisibility(
					ESlateVisibility::SelfHitTestInvisible);
				NextExpAcc += LevelUpSubsystem->GetNextLevelUpExp(curLevel + Level_WantToUP);
				/*AttributePreviewWidgets[EAttributePointType::NEXT_EXP]->TextBlock_Next->SetText(
					FText::AsNumber(LevelUpSubsystem->GetNextLevelUpExp(curLevel + Level_WantToUP)));*/
				AttributePreviewWidgets[EAttributePointType::NEXT_EXP]->TextBlock_Cur->SetText(FText::AsNumber(LevelUpSubsystem->GetNextLevelUpExp(curLevel + Level_WantToUP)));
				AttributePreviewWidgets[EAttributePointType::NEXT_EXP]->TextBlock_Next->SetText(
					FText::AsNumber(NextExpAcc));
			}

			//레벨을 더 올릴수 없으면, 레벨업 관련 부분을 숨깁니다.
			if (LevelUpSubsystem->CanLevelUp(attComp, curLevel, curLevel + Level_WantToUP + 1) == false)
			{
				
				HiddenAddButton();
			}
			else
			{
				//경험치 감소하고 남은 양 표시
				if (AttributePreviewWidgets.Contains(EAttributePointType::EXP))
				{
					const float needExpSum = LevelUpSubsystem->GetNextLevelUpExp(curLevel, curLevel + Level_WantToUP);
					AttributePreviewWidgets[EAttributePointType::EXP]->HorizontalBox_Next->SetVisibility(
						ESlateVisibility::SelfHitTestInvisible);
					AttributePreviewWidgets[EAttributePointType::EXP]->TextBlock_Next->SetText(
						FText::AsNumber(attComp->GetEXP() - needExpSum));
				}
			}
		}
		else
		{
			UE_LOGFMT(LogTemp, Log, "경험치 모잘라서 이제 더 몬올림");
			HiddenAddButton();
		}
	}
}

void ULevelUpWidget::HiddenAddButton()
{
	for (auto iter : AttributePreviewWidgets)
	{
		iter.Value->Button_Add->SetVisibility(ESlateVisibility::Hidden);
	}
}

USimpleToolTipWidget* ULevelUpWidget::CreateSimpleToolTipWidget(FText ToolTipTextToSet)
{
	if (SimpleToolTipWidgetObject)
	{
		auto widget = CreateWidget<USimpleToolTipWidget>(GetOwningPlayer(), SimpleToolTipWidgetObject);
		widget->SetDescriptionText(ToolTipTextToSet);

		return widget;
	}
	return nullptr;
}
