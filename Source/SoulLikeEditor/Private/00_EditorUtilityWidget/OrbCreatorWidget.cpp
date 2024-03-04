// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/OrbCreatorWidget.h"


#include "EditorAssetLibrary.h"
#include "PackageHelperFunctions.h"
#include "00_EditorUtilityWidget/OrbMatrixButtonWidget.h"
#include "95_OrbCreator/OrbMatrix.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/UniformGridPanel.h"
#include "Engine/ObjectLibrary.h"
#include "Logging/StructuredLog.h"
#include "Misc/MessageDialog.h"

void UOrbCreatorWidget::OnTextChangedEvent(const FText& Text)
{
	{
		FString textStr = Text.ToString();
		if (textStr.IsNumeric() == false)
		{
			EditableTextBox_Matrix->SetText(FText::GetEmpty());
			return;
		}
	}

	{
		FString textStr = Text.ToString();
		if (textStr.IsNumeric())
		{
			Matrix = FCString::Atoi(*textStr);
			if (Matrix % 2 != 0)
			{
				UniformGridPanel_Orb->ClearChildren();
			}
		}
	}
}

void UOrbCreatorWidget::OnClickedOrbMatrixButton()
{
	if (HorizontalBox_SlotType)
	{
		HorizontalBox_SlotType->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UOrbCreatorWidget::OnClickedButtonGenerateEvent()
{
	if (CurSelectedObject.IsValid() == false)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("먼저 메트릭스 오브젝트를 선택해야 합니다.")));
		return;
	}

	if (Matrix % 2 == 0)
	{
		Matrix = 0;
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("홀수 단위만 입력 가능합니다.")));
		return;
	}

	CreateGrid();
}

void UOrbCreatorWidget::OnClickedButtonClearEvent()
{
	if (EditableTextBox_Matrix)
	{
		EditableTextBox_Matrix->SetText(FText::GetEmpty());
	}

	Matrix = 0;


	if (UniformGridPanel_Orb)
	{
		UniformGridPanel_Orb->ClearChildren();
	}

	SetUnCheckOthers(CheckBox_Empty);
	CurClickedButton = nullptr;
}

void UOrbCreatorWidget::OnCheckPhysical(bool bIsChecked)
{
	SetUnCheckOthers(CheckBox_Physical);
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetPoint(EOrbMatrixSlotType::PHYSICAL);
	}
}

void UOrbCreatorWidget::OnCheckDefence(bool bIsChecked)
{
	SetUnCheckOthers(CheckBox_Defence);
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetPoint(EOrbMatrixSlotType::DEFENCE);
	}
}

void UOrbCreatorWidget::OnCheckMagical(bool bIsChecked)
{
	SetUnCheckOthers(CheckBox_Magical);
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetPoint(EOrbMatrixSlotType::MAGICAL);
	}
}

void UOrbCreatorWidget::OnCheckFree(bool bIsChecked)
{
	SetUnCheckOthers(CheckBox_Free);
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetPoint(EOrbMatrixSlotType::FREE);
	}
}

void UOrbCreatorWidget::OnCheckLine(bool bIsChecked)
{
	SetUnCheckOthers(CheckBox_Line);
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetPoint(EOrbMatrixSlotType::LINE);
	}
}

void UOrbCreatorWidget::OnCheckEmpty(bool bIsChecked)
{
	SetUnCheckOthers(CheckBox_Empty);
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetPoint(EOrbMatrixSlotType::NONE);
	}
}

void UOrbCreatorWidget::OnCheckLock(bool bIsChecked)
{
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetLocked(bIsChecked);
	}
}

void UOrbCreatorWidget::SetUnCheckOthers(UCheckBox* One)
{
	for (auto c : CheckBoxes)
	{
		if (c != One)
		{
			c->SetCheckedState(ECheckBoxState::Unchecked);
		}
	}

	if (One == CheckBox_Line)
	{
		HorizontalBox_Line->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		HorizontalBox_Line->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UOrbCreatorWidget::OnClickedLeftToRight()
{
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetLine(EOrbMatrixSlotLineDirection::Horizontal);
	}
}

void UOrbCreatorWidget::OnClickedUpToDown()
{
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetLine(EOrbMatrixSlotLineDirection::Vertical);
	}
}

void UOrbCreatorWidget::OnClickedLeftDownToRightUp()
{
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetLine(EOrbMatrixSlotLineDirection::RightUpToLeftDown);
	}
}

void UOrbCreatorWidget::OnClickedLeftUpToRightDown()
{
	if (CurClickedButton.IsValid())
	{
		CurClickedButton->SetLine(EOrbMatrixSlotLineDirection::LeftUpToRightDown);
	}
}

void UOrbCreatorWidget::OnOrbLineListSelectedChangedEvent(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (OrbSlotLineObjects.Contains(SelectedItem))
	{
		OnClickedButtonClearEvent();
		SetSelectedObjectInfo(OrbSlotLineObjects[SelectedItem]);
	}
}

void UOrbCreatorWidget::SetSelectedObjectInfo(UOrbMatrix* Value)
{
	CurSelectedObject = Value;
	if (Value)
	{
		//TODO 여기서 정보를 불러와 설정합니다.
		auto& matrix = Value->OrbMatrix;
		Matrix = matrix.Num();
		//UE_LOGFMT(LogTemp, Warning, "불러온 메트릭스의 행렬 : {0}", Matrix);
		CreateGrid();

		/*
		UE_LOGFMT(LogTemp, Warning, "--------------불러온 메트릭스의 행렬구조------------");
		for (auto i = 0; i < matrix.Num(); i++)
		{
			FString msg;
			for (auto j = 0; j < matrix[i].Num(); j++)
			{
				if (matrix[i][j].OrbMatrixSlotType != EOrbMatrixSlotType::LINE)
				{
					msg += StaticEnum<EOrbMatrixSlotType>()->GetValueAsString(matrix[i][j].OrbMatrixSlotType) + "\t";
				}
				else
				{
					msg += StaticEnum<EOrbMatrixSlotLineDirection>()->GetValueAsString(matrix[i][j].SlotLineDirection) +
						"\t";
				}
			}
			UE_LOGFMT(LogTemp, Warning, "{0}", msg);
		}
		UE_LOGFMT(LogTemp, Warning, "--------------------------------------------");
		*/

		for (auto i = 0; i < matrix.Num(); i++)
		{
			if (matrix.IsValidIndex(i))
			{
				for (auto j = 0; j < matrix[i].Num(); j++)
				{
					ButtonArray[i][j]->ForceUpdate(matrix[i][j].SlotLineDirection, matrix[i][j].OrbMatrixSlotType,
					                               matrix[i][j].bLock);
				}
			}
		}
	}
}

void UOrbCreatorWidget::CreateGrid()
{
	if (OrbMatrixButtonObject)
	{
		int32 half = Matrix / 2;

		for (auto i = 0; i < ButtonArray.Num(); i++)
		{
			if (ButtonArray.IsValidIndex(i))
			{
				ButtonArray[i].Empty();
			}
		}
		ButtonArray.Empty();

		//?�?�용 2차원 배열 초기??
		for (auto i = 0; i < Matrix; i++)
		{
			ButtonArray.Add(TArray<UOrbMatrixButtonWidget*>());
			for (auto j = 0; j < Matrix; j++)
			{
				ButtonArray[i].Add(nullptr);
			}
		}

		for (auto i = 0; i < Matrix; i++)
		{
			for (auto j = 0; j < Matrix; j++)
			{
				if (auto button = CreateWidget<UOrbMatrixButtonWidget>(this, OrbMatrixButtonObject))
				{
					button->OrbCreatorWidget = this;
					if (i == half && j == half)
					{
						button->SetCore();
					}
					button->Button->OnClicked.AddUniqueDynamic(this, &UOrbCreatorWidget::OnClickedOrbMatrixButton);

					button->SetColRow(i, j);
					ButtonArray[i][j] = button;
					UniformGridPanel_Orb->AddChildToUniformGrid(button, i, j);
				}
			}
		}
	}
}

void UOrbCreatorWidget::GetLine(int32 i, int32 j, TArray<FOrbMatrixElementInfo>& LineArray,
                                const EOrbMatrixSlotType& SlotType)
{
	if (ButtonArray.IsValidIndex(i) == false)
	{
		return;
	}

	if (ButtonArray[i].IsValidIndex(j) == false)
	{
		return;
	}

	//UE_LOGFMT(LogTemp,Log,"{0}행{1}열 확인",i,j);
	auto IsContain = [LineArray](const int32& row, const int32& col)
	{
		return LineArray.ContainsByPredicate([col, row](const FOrbMatrixElementInfo& a)
		{
			return a.Col == col && a.Row == row;
		});
	};

	//상
	if (ButtonArray.IsValidIndex(i - 1))
	{
		if (ButtonArray[i - 1].IsValidIndex(j))
		{
			//위로 연결된 라인이 있는 경우
			if (ButtonArray[i - 1][j]->OrbMatrixSlotType == EOrbMatrixSlotType::LINE && ButtonArray[i - 1][j]->
				LineDirection == EOrbMatrixSlotLineDirection::Vertical)
			{
				auto button = ButtonArray[i - 2][j];
				if (button->OrbMatrixSlotType == SlotType)
				{
					if (IsContain(i - 2, j) == false)
					{
						LineArray.AddUnique(FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                          button->SlotContent, i - 2, j, button->bLock));

						GetLine(i - 2, j, LineArray, SlotType);
					}
				}
			}
		}
	}

	//좌상
	if (ButtonArray.IsValidIndex(i - 1))
	{
		if (ButtonArray[i - 1].IsValidIndex(j - 1))
		{
			//좌상으로 연결된 라인이 있는 경우
			if (ButtonArray[i - 1][j - 1]->OrbMatrixSlotType == EOrbMatrixSlotType::LINE && ButtonArray[i - 1][j - 1]->
				LineDirection == EOrbMatrixSlotLineDirection::LeftUpToRightDown)
			{
				auto button = ButtonArray[i - 2][j - 2];
				if (button->OrbMatrixSlotType == SlotType)
				{
					if (IsContain(i - 2, j - 2) == false)
					{
						LineArray.AddUnique(FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                          button->SlotContent, i - 2, j - 2, button->bLock));

						GetLine(i - 2, j - 2, LineArray, SlotType);
					}
				}
			}
		}
	}

	//좌
	if (ButtonArray.IsValidIndex(i))
	{
		if (ButtonArray[i].IsValidIndex(j - 1))
		{
			//좌상으로 연결된 라인이 있는 경우
			if (ButtonArray[i][j - 1]->OrbMatrixSlotType == EOrbMatrixSlotType::LINE && ButtonArray[i][j - 1]->
				LineDirection == EOrbMatrixSlotLineDirection::Horizontal)
			{
				auto button = ButtonArray[i][j - 2];
				if (button->OrbMatrixSlotType == SlotType)
				{
					if (IsContain(i, j - 2) == false)
					{
						LineArray.AddUnique(FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                          button->SlotContent, i, j - 2, button->bLock));

						GetLine(i, j - 2, LineArray, SlotType);
					}
				}
			}
		}
	}

	//좌하
	if (ButtonArray.IsValidIndex(i + 1))
	{
		if (ButtonArray[i + 1].IsValidIndex(j - 1))
		{
			//좌하로 연결된 라인이 있는 경우
			if (ButtonArray[i + 1][j - 1]->OrbMatrixSlotType == EOrbMatrixSlotType::LINE && ButtonArray[i + 1][j - 1]->
				LineDirection == EOrbMatrixSlotLineDirection::RightUpToLeftDown)
			{
				auto button = ButtonArray[i + 2][j - 2];
				if (button->OrbMatrixSlotType == SlotType)
				{
					if (IsContain(i + 2, j - 2) == false)
					{
						LineArray.AddUnique(FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                          button->SlotContent, i + 2, j - 2, button->bLock));

						GetLine(i + 2, j - 2, LineArray, SlotType);
					}
				}
			}
		}
	}

	//하
	if (ButtonArray.IsValidIndex(i + 1))
	{
		if (ButtonArray[i + 1].IsValidIndex(j))
		{
			//좌하로 연결된 라인이 있는 경우
			if (ButtonArray[i + 1][j]->OrbMatrixSlotType == EOrbMatrixSlotType::LINE && ButtonArray[i + 1][j]->
				LineDirection == EOrbMatrixSlotLineDirection::Vertical)
			{
				auto button = ButtonArray[i + 2][j];
				if (button->OrbMatrixSlotType == SlotType)
				{
					if (IsContain(i + 2, j) == false)
					{
						LineArray.AddUnique(FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                          button->SlotContent, i + 2, j, button->bLock));

						GetLine(i + 2, j, LineArray, SlotType);
					}
				}
			}
		}
	}

	//우하
	if (ButtonArray.IsValidIndex(i + 1))
	{
		if (ButtonArray[i + 1].IsValidIndex(j + 1))
		{
			//좌하로 연결된 라인이 있는 경우
			if (ButtonArray[i + 1][j + 1]->OrbMatrixSlotType == EOrbMatrixSlotType::LINE && ButtonArray[i + 1][j + 1]->
				LineDirection == EOrbMatrixSlotLineDirection::LeftUpToRightDown)
			{
				auto button = ButtonArray[i + 2][j + 2];
				if (button->OrbMatrixSlotType == SlotType)
				{
					if (IsContain(i + 2, j + 2) == false)
					{
						LineArray.AddUnique(FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                          button->SlotContent, i + 2, j + 2, button->bLock));

						GetLine(i + 2, j + 2, LineArray, SlotType);
					}
				}
			}
		}
	}

	//우
	if (ButtonArray.IsValidIndex(i))
	{
		if (ButtonArray[i].IsValidIndex(j + 1))
		{
			//좌하로 연결된 라인이 있는 경우
			if (ButtonArray[i][j + 1]->OrbMatrixSlotType == EOrbMatrixSlotType::LINE && ButtonArray[i][j + 1]->
				LineDirection == EOrbMatrixSlotLineDirection::Horizontal)
			{
				auto button = ButtonArray[i][j + 2];
				if (button->OrbMatrixSlotType == SlotType)
				{
					if (IsContain(i, j + 2) == false)
					{
						LineArray.AddUnique(FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                          button->SlotContent, i, j + 2, button->bLock));

						GetLine(i, j + 2, LineArray, SlotType);
					}
				}
			}
		}
	}

	//우상
	if (ButtonArray.IsValidIndex(i - 1))
	{
		if (ButtonArray[i - 1].IsValidIndex(j + 1))
		{
			//우상으로 연결된 라인이 있는 경우
			if (ButtonArray[i - 1][j + 1]->OrbMatrixSlotType == EOrbMatrixSlotType::LINE && ButtonArray[i - 1][j + 1]->
				LineDirection == EOrbMatrixSlotLineDirection::RightUpToLeftDown)
			{
				auto button = ButtonArray[i - 2][j + 2];
				if (button->OrbMatrixSlotType == SlotType)
				{
					if (IsContain(i - 2, j + 2) == false)
					{
						LineArray.AddUnique(FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                          button->SlotContent, i - 2, j + 2, button->bLock));

						GetLine(i - 2, j + 2, LineArray, SlotType);
					}
				}
			}
		}
	}
}

void UOrbCreatorWidget::OnClickedSaveButton()
{
	if (CurSelectedObject.IsValid())
	{
		TArray<FMatrixArray> matrix;
		TArray<FOrbMatrixElementInfo> strLine;
		TArray<FOrbMatrixElementInfo> dexLine;
		TArray<FOrbMatrixElementInfo> intLine;
		TArray<FOrbMatrixElementInfo> freeLine;

		const int32 coreIndex = ButtonArray.Num() / 2;
		GetLine(coreIndex, coreIndex, strLine, EOrbMatrixSlotType::PHYSICAL);
		GetLine(coreIndex, coreIndex, dexLine, EOrbMatrixSlotType::DEFENCE);
		GetLine(coreIndex, coreIndex, intLine, EOrbMatrixSlotType::MAGICAL);
		GetLine(coreIndex, coreIndex, freeLine, EOrbMatrixSlotType::FREE);

		UE_LOGFMT(LogTemp, Error, "힘 라인 체크");

		for (auto e : strLine)
		{
			UE_LOGFMT(LogTemp, Error, "{0} {1}", e.Row, e.Col);
		}

		UE_LOGFMT(LogTemp, Error, "민 라인 체크");

		for (auto e : dexLine)
		{
			UE_LOGFMT(LogTemp, Error, "{0} {1}", e.Row, e.Col);
		}


		UE_LOGFMT(LogTemp, Error, "지 라인 체크");

		for (auto e : intLine)
		{
			UE_LOGFMT(LogTemp, Error, "{0} {1}", e.Row, e.Col);
		}


		UE_LOGFMT(LogTemp, Error, "자유 라인 체크");

		for (auto e : freeLine)
		{
			UE_LOGFMT(LogTemp, Error, "{0} {1}", e.Row, e.Col);
		}


		for (auto i = 0; i < ButtonArray.Num(); i++)
		{
			matrix.Add(FMatrixArray());

			if (ButtonArray.IsValidIndex(i))
			{
				for (auto j = 0; j < ButtonArray[i].Num(); j++)
				{
					if (const auto button = ButtonArray[i][j])
					{
						auto info = FOrbMatrixElementInfo(button->OrbMatrixSlotType, button->LineDirection,
						                                  button->SlotContent, i, j, button->bLock);
						matrix[i].Add(info);
					}
				}
			}
		}

		CurSelectedObject->SaveInfo(matrix, strLine, dexLine, intLine, freeLine);
		CurSelectedObject->MarkPackageDirty();


		if (!UEditorAssetLibrary::SaveLoadedAsset(CurSelectedObject.Get()))
		{
			UE_LOGFMT(LogTemp, Log, "저장에 성공했다. : {0}", CurSelectedObject->GetPathName());
		}
	}
}


void UOrbCreatorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (EditableTextBox_Matrix)
	{
		EditableTextBox_Matrix->OnTextChanged.AddUniqueDynamic(this, &UOrbCreatorWidget::OnTextChangedEvent);
		EditableTextBox_Matrix->SetText(FText::AsNumber(15));
	}

	if (Button_Generate)
	{
		Button_Generate->OnClicked.AddUniqueDynamic(this, &UOrbCreatorWidget::OnClickedButtonGenerateEvent);
	}

	if (Button_Clear)
	{
		Button_Clear->OnClicked.AddUniqueDynamic(this, &UOrbCreatorWidget::OnClickedButtonClearEvent);
	}

	if (Button_Save)
	{
		Button_Save->OnClicked.AddUniqueDynamic(this, &UOrbCreatorWidget::OnClickedSaveButton);
	}

	CheckBoxes.AddUnique(CheckBox_Physical);
	CheckBoxes.AddUnique(CheckBox_Defence);
	CheckBoxes.AddUnique(CheckBox_Magical);
	CheckBoxes.AddUnique(CheckBox_Free);
	CheckBoxes.AddUnique(CheckBox_Line);
	CheckBoxes.AddUnique(CheckBox_Empty);
	//CheckBoxes.AddUnique(CheckBox_Lock);

	if (CheckBox_Physical)
	{
		CheckBox_Physical->OnCheckStateChanged.AddUniqueDynamic(this, &UOrbCreatorWidget::OnCheckPhysical);
	}
	if (CheckBox_Defence)
	{
		CheckBox_Defence->OnCheckStateChanged.AddUniqueDynamic(this, &UOrbCreatorWidget::OnCheckDefence);
	}
	if (CheckBox_Magical)
	{
		CheckBox_Magical->OnCheckStateChanged.AddUniqueDynamic(this, &UOrbCreatorWidget::OnCheckMagical);
	}
	if (CheckBox_Free)
	{
		CheckBox_Free->OnCheckStateChanged.AddUniqueDynamic(this, &UOrbCreatorWidget::OnCheckFree);
	}
	if (CheckBox_Line)
	{
		CheckBox_Line->OnCheckStateChanged.AddUniqueDynamic(this, &UOrbCreatorWidget::OnCheckLine);
	}
	if (CheckBox_Empty)
	{
		CheckBox_Empty->OnCheckStateChanged.AddUniqueDynamic(this, &UOrbCreatorWidget::OnCheckEmpty);
	}
	if (CheckBox_Lock)
	{
		CheckBox_Lock->OnCheckStateChanged.AddUniqueDynamic(this, &UOrbCreatorWidget::OnCheckLock);
	}

	/*
	if(Button_Load)
	{
		Button_Load->OnClicked.AddUniqueDynamic(this,&UOrbCreatorWidget::OnClickedLoad);
	}

	if(Button_New)
	{
		Button_New->OnClicked.AddUniqueDynamic(this,&UOrbCreatorWidget::OnClickedNew);
	}*/

	if (Button_LeftToRight)
	{
		Button_LeftToRight->OnClicked.AddUniqueDynamic(this, &UOrbCreatorWidget::OnClickedLeftToRight);
	}

	if (Button_UpToDown)
	{
		Button_UpToDown->OnClicked.AddUniqueDynamic(this, &UOrbCreatorWidget::OnClickedUpToDown);
	}

	if (Button_LeftDownToRightUp)
	{
		Button_LeftDownToRightUp->OnClicked.AddUniqueDynamic(this, &UOrbCreatorWidget::OnClickedLeftDownToRightUp);
	}

	if (Button_LeftUpToRightDown)
	{
		Button_LeftUpToRightDown->OnClicked.AddUniqueDynamic(this, &UOrbCreatorWidget::OnClickedLeftUpToRightDown);
	}

	if (ComboBoxString_OrbMatrixList)
	{
		ComboBoxString_OrbMatrixList->OnSelectionChanged.AddUniqueDynamic(
			this, &UOrbCreatorWidget::OnOrbLineListSelectedChangedEvent);
	}
}

void UOrbCreatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (const auto asset : LoadOrbMatrix())
	{
		//UE_LOGFMT(LogTemp,Log,"불러온 애셋 이름 : {0}",asset->GetName());
		OrbSlotLineObjects.Add(asset->GetName(), Cast<UOrbMatrix>(asset));
		ComboBoxString_OrbMatrixList->AddOption(asset->GetName());
	}
}

void UOrbCreatorWidget::SetButtonInfo(UOrbMatrixButtonWidget* Button)
{
	if (Button)
	{
		CheckBox_Lock->SetIsChecked(Button->bLock);
		UE_LOGFMT(LogTemp, Error, "슬롯 정보 : {0}, 잠금여부 : {1}",
		          StaticEnum<EOrbMatrixSlotType>()->GetValueAsString(Button->OrbMatrixSlotType),
		          CheckBox_Lock->IsChecked());

		switch (Button->OrbMatrixSlotType)
		{
		case EOrbMatrixSlotType::NONE:
			CheckBox_Empty->SetCheckedState(ECheckBoxState::Checked);
			SetUnCheckOthers(CheckBox_Empty);
			break;
		case EOrbMatrixSlotType::LINE:
			CheckBox_Line->SetCheckedState(ECheckBoxState::Checked);
			SetUnCheckOthers(CheckBox_Line);
			break;
		case EOrbMatrixSlotType::CORE:
			CheckBox_Empty->SetCheckedState(ECheckBoxState::Checked);
			SetUnCheckOthers(CheckBox_Empty);
			break;
		case EOrbMatrixSlotType::PHYSICAL:
			CheckBox_Physical->SetCheckedState(ECheckBoxState::Checked);
			SetUnCheckOthers(CheckBox_Physical);
			break;
		case EOrbMatrixSlotType::DEFENCE:
			CheckBox_Defence->SetCheckedState(ECheckBoxState::Checked);
			SetUnCheckOthers(CheckBox_Defence);
			break;
		case EOrbMatrixSlotType::MAGICAL:
			CheckBox_Magical->SetCheckedState(ECheckBoxState::Checked);
			SetUnCheckOthers(CheckBox_Magical);
			break;
		case EOrbMatrixSlotType::FREE:
			CheckBox_Free->SetCheckedState(ECheckBoxState::Checked);
			SetUnCheckOthers(CheckBox_Free);
			break;
		default: ;
		}
	}
}

TArray<UObject*> UOrbCreatorWidget::LoadOrbMatrix()
{
	TArray<UObject*> asset;


	// 블루프린트 애셋 데이터를 가져온다.
	TArray<FAssetData> AssetData;

	const FAssetRegistryModule& AssetRegistry =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FARFilter filter;
	filter.bRecursiveClasses = true;
	filter.ClassPaths.Add(FTopLevelAssetPath(UOrbMatrix::StaticClass()));
	AssetRegistry.Get().GetAssets(filter, AssetData, true);

	for (const auto& d : AssetData)
	{
		asset.Add(Cast<UOrbMatrix>(d.GetAsset()));
	}


	return asset;
}
