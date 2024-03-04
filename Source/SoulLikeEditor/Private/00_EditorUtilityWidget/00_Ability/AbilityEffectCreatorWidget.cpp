// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/00_Ability/AbilityEffectCreatorWidget.h"


#include "02_Library/BlueprintHelperLibrary.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/DetailsView.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Logging/StructuredLog.h"


void UAbilityEffectCreatorWidget::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	SelectedType = SelectedItem;

	if (SelectedItem.Equals(TEXT("일반")))
	{
		DetailsView_Effect->SetObject(NewObject<UNormalEffectInfo>());
	}
	else
	{
		DetailsView_Effect->SetObject(NewObject<ULinetraceEffectInfo>());
	}
}

void UAbilityEffectCreatorWidget::OnClicked()
{
	auto path = TextBlock_SelectPath->GetText().ToString();
	if (path.IsEmpty())
	{
		UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString(TEXT("경로가 비었습니다.")));
		return;
	}

	auto name = EditableTextBox_BlueprintName->GetText().ToString();
	if (name.IsEmpty())
	{
		UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString(TEXT("블루프린트 이름이 비었습니다.")));
		return;
	}

	const auto fullPath = path + "/" + name;
	UE_LOGFMT(LogTemp, Warning, "최종 경로 : {0}", fullPath);


	if (auto data = DetailsView_Effect->GetObject())
	{
		bool bSucc;
		FString msg;
		UBlueprint* bp;

		if (SelectedType.Equals(TEXT("일반")))
		{
			auto normalInfo = Cast<UNormalEffectInfo>(data);

			bp = UBlueprintHelperLibrary::CreateBlueprint(fullPath, normalInfo->Effect->GetClass(), bSucc, msg);
			if (bSucc == false)
			{
				UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString(msg));
			}

			Cast<UAbilityEffect>(bp->GeneratedClass.GetDefaultObject())->CopyValues(normalInfo->Effect);
		}
		else
		{
			auto linetraceInfo = Cast<ULinetraceEffectInfo>(data);
			bp = UBlueprintHelperLibrary::CreateBlueprint(fullPath, linetraceInfo->Effect->GetClass(), bSucc, msg);
			if (bSucc == false)
			{
				UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString(msg));
			}
			Cast<UAbilityEffect>(bp->GeneratedClass.GetDefaultObject())->CopyValues(linetraceInfo->Effect);
		}

		bp->MarkPackageDirty();
	}
}

void UAbilityEffectCreatorWidget::OnClickedPath()
{
	TextBlock_SelectPath->SetText(FText::FromString(UBlueprintHelperLibrary::OpenDirectoryDialog()));
}

void UAbilityEffectCreatorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	/*if(DetailsView_Effect)
	{
		DetailsView_Effect->SetVisibility(ESlateVisibility::Collapsed);
	}*/

	if (ComboBoxString_EffectType)
	{
		ComboBoxString_EffectType->OnSelectionChanged.AddUniqueDynamic(
			this, &UAbilityEffectCreatorWidget::OnSelectionChanged);
		//ComboBoxString_EffectType->SetSelectedOption(TEXT(""));
	}

	if (Button_Create)
	{
		Button_Create->OnClicked.AddUniqueDynamic(this, &UAbilityEffectCreatorWidget::OnClicked);
	}

	if (Button_SelectPath)
	{
		Button_SelectPath->OnClicked.AddUniqueDynamic(this, &UAbilityEffectCreatorWidget::OnClickedPath);
	}
}
