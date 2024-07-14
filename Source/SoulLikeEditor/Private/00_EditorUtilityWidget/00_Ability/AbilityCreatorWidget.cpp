// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/00_Ability/AbilityCreatorWidget.h"

#include "00_EditorUtilityWidget/00_Ability/AbilityCreatorWidget_Req.h"
#include "02_Library/BlueprintHelperLibrary.h"
#include "04_Item/ItemActor.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/DetailsView.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"

UAbilityCreatorWidget::UAbilityCreatorWidget()
{
	//("소비아이템","방어구","무기(스피릿)","장신구","코어","파편","기타")
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Consume/DT_ItemConsumeAbility.DT_ItemConsumeAbility'"));
		if (table.Succeeded())
		{
			ConsumeTable = table.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Armor/DT_ArmorAbility.DT_ArmorAbility'"));
		if (table.Succeeded())
		{
			ArmorTable = table.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Spirit/DT_SpiritAbility.DT_SpiritAbility'"));
		if (table.Succeeded())
		{
			SpiritTable = table.Object;
		}
	}
	
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Ring/DT_RingAbility.DT_RingAbility'"));
		if (table.Succeeded())
		{
			RingTable = table.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Orb/Core/DT_CoreAbility.DT_CoreAbility'"));
		if (table.Succeeded())
		{
			CoreTable = table.Object;
		}
	}

	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Orb/Fragment/DT_FragmentAbility.DT_FragmentAbility'"));
		if (table.Succeeded())
		{
			FragmentTable = table.Object;
		}
	}
	
	
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/00_Character/02_CommonAbility/DT_Ability.DT_Ability'"));
		if (table.Succeeded())
		{
			CommonTable = table.Object;
		}
	}
}

void UAbilityCreatorWidget::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (CreatableAbilityMap.Contains(SelectedItem))
	{
		SelectedClassName = SelectedItem;
	}
	else
	{
		SelectedClassName = "";
	}
}

void UAbilityCreatorWidget::OnClickedPathButton()
{
	TextBlock_SelectPath->SetText(FText::FromString(UBlueprintHelperLibrary::OpenDirectoryDialog()));
}

void UAbilityCreatorWidget::OnClickedCreateButton()
{
	if (EditableTextBox_BlueprintName->GetText().IsEmpty())
	{
		UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString(TEXT("이름이 비었습니다.")));
		return;
	}

	auto path = TextBlock_SelectPath->GetText().ToString();
	if (path.IsEmpty())
	{
		UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString(TEXT("경로가 비었습니다.")));
		return;
	}

	if (E_UMG_AbilityCreator_Req)
	{
		if (auto req = E_UMG_AbilityCreator_Req->CreateAR(path, EditableTextBox_BlueprintName->GetText().ToString()))
		{
			auto fullPath = path + "/A_" + EditableTextBox_BlueprintName->GetText().ToString();
			FString msg;
			bool bSucc;

			auto bp = UBlueprintHelperLibrary::CreateBlueprint(fullPath, UAbilityBase::StaticClass(), bSucc, msg);
			if (bSucc == false)
			{
				UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString(msg));
			}
			else
			{
				auto ability = Cast<UAbilityBase>(bp->GeneratedClass.GetDefaultObject());
				if (auto data = DetailsView_AbilityInfo->GetObject())
				{
					auto tableData = Cast<UAbilityTableInfo>(data);
					tableData->AbilityInformation.AbilityRequirement = req->GetClass();

					AddToDataTable(tableData->AbilityInformation.AbilityTag.GetTagName(),
										 tableData->AbilityInformation);
					

					ability->AbilityTag = tableData->AbilityInformation.AbilityTag;
					ability->Montages = tableData->AnimMontages;
					ability->AbilityInformationTable = CommonTable;
					ability->SetAbilityInformation();
					ability->MarkPackageDirty();
				}
			}
		}
	}
}

void UAbilityCreatorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (DetailsView_AbilityInfo)
	{
		if(AbilityInfoHandler == nullptr)
		{
			AbilityInfoHandler = NewObject<UAbilityTableInfo>();
		}
		DetailsView_AbilityInfo->SetObject(AbilityInfoHandler);
	}

	if (ComboBoxString_Class)
	{
		for (const auto c : CreatableAbilityClass)
		{
			ComboBoxString_Class->AddOption(c->GetName());
			CreatableAbilityMap.Add(c->GetName(), c);
		}

		ComboBoxString_Class->OnSelectionChanged.AddUniqueDynamic(this, &UAbilityCreatorWidget::OnSelectionChanged);
	}

	if (Button_SelectPath)
	{
		Button_SelectPath->OnClicked.AddUniqueDynamic(this, &UAbilityCreatorWidget::OnClickedPathButton);
	}

	if (Button_Create)
	{
		Button_Create->OnClicked.AddUniqueDynamic(this, &UAbilityCreatorWidget::OnClickedCreateButton);
	}
}

void UAbilityCreatorWidget::AddToDataTable(FName AbilityTag, const FAbilityInformation& AbilityInformation)
{
	/*
	*	CommonTable->AddRow(tableData->AbilityInformation.AbilityTag.GetTagName(),
	tableData->AbilityInformation);
	CommonTable->MarkPackageDirty();
	*/

	auto option = ComboBoxString_TableList->GetSelectedOption();

	if(option.IsEmpty())
	{
		UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString("어빌리티가 저장될 테이블 정보를 선택하지 않았습니다."));
		return;
	}
	
	//("소비아이템","방어구","무기(스피릿)","장신구","코어","파편","기타")
	if(option.Equals(TEXT("소비아이템")))
	{
		if(ConsumeTable->FindRow<FItemInformation>(AbilityTag,"")!=nullptr)
		{
			UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString("이미 테이블에 해당 이 태그를 키로 하는 행이 존재합니다. 키는 중복될 수 없습니다."));
			return;
		}
		
		ConsumeTable->AddRow(AbilityTag,AbilityInformation);
		ConsumeTable->MarkPackageDirty();	
	}else if(option.Equals(TEXT("방어구")))
	{
		if(ArmorTable->FindRow<FItemInformation>(AbilityTag,"")!=nullptr)
		{
			UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString("이미 테이블에 해당 이 태그를 키로 하는 행이 존재합니다. 키는 중복될 수 없습니다."));
			return;
		}
		
		ArmorTable->AddRow(AbilityTag,AbilityInformation);
		ArmorTable->MarkPackageDirty();
	}else if(option.Equals(TEXT("무기(스피릿)")))
	{
		if(SpiritTable->FindRow<FItemInformation>(AbilityTag,"")!=nullptr)
		{
			UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString("이미 테이블에 해당 이 태그를 키로 하는 행이 존재합니다. 키는 중복될 수 없습니다."));
			return;
		}
		SpiritTable->AddRow(AbilityTag,AbilityInformation);
		SpiritTable->MarkPackageDirty();
	}else if(option.Equals(TEXT("장신구")))
	{
		if(RingTable->FindRow<FItemInformation>(AbilityTag,"")!=nullptr)
		{
			UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString("이미 테이블에 해당 이 태그를 키로 하는 행이 존재합니다. 키는 중복될 수 없습니다."));
			return;
		}
		RingTable->AddRow(AbilityTag,AbilityInformation);
		RingTable->MarkPackageDirty();
	}else if(option.Equals(TEXT("코어")))
	{
		if(CoreTable->FindRow<FItemInformation>(AbilityTag,"")!=nullptr)
		{
			UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString("이미 테이블에 해당 이 태그를 키로 하는 행이 존재합니다. 키는 중복될 수 없습니다."));
			return;
		}
		CoreTable->AddRow(AbilityTag,AbilityInformation);
		CoreTable->MarkPackageDirty();
	}else if(option.Equals(TEXT("파편")))
	{
		if(FragmentTable->FindRow<FItemInformation>(AbilityTag,"")!=nullptr)
		{
			UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString("이미 테이블에 해당 이 태그를 키로 하는 행이 존재합니다. 키는 중복될 수 없습니다."));
			return;
		}
		FragmentTable->AddRow(AbilityTag,AbilityInformation);
		FragmentTable->MarkPackageDirty();
	}else if(option.Equals(TEXT("기타")))
	{
		if(CommonTable->FindRow<FItemInformation>(AbilityTag,"")!=nullptr)
		{
			UBlueprintHelperLibrary::ShowAlertDialog(FText::FromString("이미 테이블에 해당 이 태그를 키로 하는 행이 존재합니다. 키는 중복될 수 없습니다."));
			return;
		}
		CommonTable->AddRow(AbilityTag,AbilityInformation);
		CommonTable->MarkPackageDirty();
	}
}
