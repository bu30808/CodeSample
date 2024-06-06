// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/ItemCreatorWidget.h"


#include "02_Library/BlueprintHelperLibrary.h"
#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/ConsumeItemActor.h"
#include "04_Item/01_Equipment/00_Spirit/EquipmentItemActor_Spirit.h"
#include "04_Item/01_Equipment/01_Armor/EquipmentItemActor_Armor.h"
#include "04_Item/01_Equipment/02_Ring/EquipmentItemActor_Ring.h"
#include "04_Item/01_Equipment/03_Orb/EquipmentItemActor_OrbCore.h"
#include "04_Item/01_Equipment/03_Orb/EquipmentItemActor_OrbFragment.h"
#include "04_Item/02_Enhancement/EnhancementItemActor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/DetailsView.h"
#include "Components/EditableTextBox.h"
#include "Components/SinglePropertyView.h"
#include "Components/TextBlock.h"
#include "Misc/MessageDialog.h"
#include "UObject/ConstructorHelpers.h"

UItemCreatorWidget::UItemCreatorWidget()
{
	bAutoRunDefaultAction = true;

	//무기
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Spirit/DT_Spirit.DT_Spirit'"));
		if (table.Succeeded())
		{
			SpiritTable = table.Object;
		}

		static ConstructorHelpers::FObjectFinder<UDataTable> abilityTable(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Spirit/DT_SpiritAbility.DT_SpiritAbility'"));
		if (abilityTable.Succeeded())
		{
			SpiritAbilityTable = abilityTable.Object;
		}
	}

	//방어구
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Armor/DT_Armor.DT_Armor'"));
		if (table.Succeeded())
		{
			ArmorTable = table.Object;
		}

		static ConstructorHelpers::FObjectFinder<UDataTable> abilityTable(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Armor/DT_ArmorAbility.DT_ArmorAbility'"));
		if (abilityTable.Succeeded())
		{
			ArmorAbilityTable = abilityTable.Object;
		}
	}

	//장신구
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Ring/DT_Ring.DT_Ring'"));
		if (table.Succeeded())
		{
			RingTable = table.Object;
		}

		static ConstructorHelpers::FObjectFinder<UDataTable> abilityTable(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Ring/DT_RingAbility.DT_RingAbility'"));
		if (abilityTable.Succeeded())
		{
			RingAbilityTable = abilityTable.Object;
		}
	}

	//코어
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Orb/Core/DT_OrbCore.DT_OrbCore'"));
		if (table.Succeeded())
		{
			CoreTable = table.Object;
		}

		static ConstructorHelpers::FObjectFinder<UDataTable> abilityTable(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Orb/Core/DT_CoreAbility.DT_CoreAbility'"));
		if (abilityTable.Succeeded())
		{
			CoreAbilityTable = abilityTable.Object;
		}
	}

	//소비
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> table(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Consume/DT_Consume.DT_Consume'"));
		if (table.Succeeded())
		{
			ConsumeTable = table.Object;
		}

		static ConstructorHelpers::FObjectFinder<UDataTable> abilityTable(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Consume/DT_ItemConsumeAbility.DT_ItemConsumeAbility'"));
		if (abilityTable.Succeeded())
		{
			ConsumeAbilityTable = abilityTable.Object;
		}
	}

	//강화
	{
		static ConstructorHelpers::FObjectFinder<UDataTable> enhancementItemTable(TEXT(
			"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Enhancement/DT_EnhancementItem.DT_EnhancementItem'"));
		if (enhancementItemTable.Succeeded())
		{
			EnhancementItemTable = enhancementItemTable.Object;
		}
	}
}

void UItemCreatorWidget::OnClickedPathButton()
{
	TextBlock_SelectPath->SetText(FText::FromString(UBlueprintHelperLibrary::OpenDirectoryDialog()));
}

void UItemCreatorWidget::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (CreatableItemActorMap.Contains(SelectedItem))
	{
		SelectedClassName = SelectedItem;

		if (CreatableItemActorMap[SelectedItem] == AEquipmentItemActor_Spirit::StaticClass())
		{
			DetailsView_Item->SetObject(NewObject<USpiritInfo>());
		}

		if (CreatableItemActorMap[SelectedItem] == AEquipmentItemActor_Armor::StaticClass())
		{
			DetailsView_Item->SetObject(NewObject<UEquipInfo>());
		}

		if (CreatableItemActorMap[SelectedItem] == AEquipmentItemActor_Ring::StaticClass())
		{
			DetailsView_Item->SetObject(NewObject<UEquipInfo>());
		}

		if (CreatableItemActorMap[SelectedItem] == AEquipmentItemActor_OrbCore::StaticClass())
		{
			DetailsView_Item->SetObject(NewObject<UCoreInfo>());
		}

		if (CreatableItemActorMap[SelectedItem] == AEquipmentItemActor_OrbFragment::StaticClass())
		{
			DetailsView_Item->SetObject(NewObject<UFragmentInfo>());
		}

		if (CreatableItemActorMap[SelectedItem] == AConsumeItemActor::StaticClass())
		{
			DetailsView_Item->SetObject(NewObject<UConsumeInfo>());
		}

		if (CreatableItemActorMap[SelectedItem] == AEnhancementItemActor::StaticClass())
		{
			DetailsView_Item->SetObject(NewObject<UCommonInfo>());
		}
	}
	else
	{
		SelectedClassName = "";
	}
}

void UItemCreatorWidget::AddTableInfo(UBlueprint* ItemBlueprint)
{
	if (auto data = DetailsView_Item->GetObject())
	{
		if (CreatableItemActorMap.Contains(SelectedClassName))
		{
			if (CreatableItemActorMap[SelectedClassName] == AEquipmentItemActor_Spirit::StaticClass())
			{
				auto info = Cast<USpiritInfo>(data)->WeaponInformation;
				info.ItemActorObject = ItemBlueprint->GeneratedClass.GetDefaultObject()->GetClass();

				if (info.Item_Tag.IsValid() == false)
				{
					FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("아이템 태그가 비어있으면 안 됩니다.")));
					return;
				}

				SpiritTable->AddRow(info.Item_Tag.GetTagName(), info);
				SpiritTable->MarkPackageDirty();
			}

			if (CreatableItemActorMap[SelectedClassName] == AEquipmentItemActor_Armor::StaticClass())
			{
				auto info = Cast<UEquipInfo>(data)->EquipInformation;
				info.ItemActorObject = ItemBlueprint->GeneratedClass.GetDefaultObject()->GetClass();

				if (info.Item_Tag.IsValid() == false)
				{
					FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("아이템 태그가 비어있으면 안 됩니다.")));
					return;
				}

				ArmorTable->AddRow(info.Item_Tag.GetTagName(), info);
				ArmorTable->MarkPackageDirty();
			}

			if (CreatableItemActorMap[SelectedClassName] == AEquipmentItemActor_Ring::StaticClass())
			{
				auto info = Cast<UEquipInfo>(data)->EquipInformation;
				info.ItemActorObject = ItemBlueprint->GeneratedClass.GetDefaultObject()->GetClass();

				if (info.Item_Tag.IsValid() == false)
				{
					FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("아이템 태그가 비어있으면 안 됩니다.")));
					return;
				}

				RingTable->AddRow(info.Item_Tag.GetTagName(), info);
				RingTable->MarkPackageDirty();
			}

			if (CreatableItemActorMap[SelectedClassName] == AEquipmentItemActor_OrbCore::StaticClass())
			{
				auto info = Cast<UCoreInfo>(data)->OrbCoreInformation;
				info.ItemActorObject = ItemBlueprint->GeneratedClass.GetDefaultObject()->GetClass();

				if (info.Item_Tag.IsValid() == false)
				{
					FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("아이템 태그가 비어있으면 안 됩니다.")));
					return;
				}

				CoreTable->AddRow(info.Item_Tag.GetTagName(), info);
				CoreTable->MarkPackageDirty();
			}

			if (CreatableItemActorMap[SelectedClassName] == AEquipmentItemActor_OrbFragment::StaticClass())
			{
				//아직 파편 테이블은 없습니다.

				/*
				auto info = Cast<UFragmentInfo>(data)->OrbFragmentInformation;
				info.ItemActorObject = ItemBlueprint->GeneratedClass.GetDefaultObject()->GetClass();
				
				if(info.Item_Tag.IsValid()==false)
				{
					FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("아이템 태그가 비어있으면 안 됩니다.")));
					return;
				}
				
				SpiritTable->AddRow(info.Item_Tag.GetTagName(),info);*/
			}

			if (CreatableItemActorMap[SelectedClassName] == AConsumeItemActor::StaticClass())
			{
				auto info = Cast<UConsumeInfo>(data)->ConsumeInformation;
				info.ItemActorObject = ItemBlueprint->GeneratedClass.GetDefaultObject()->GetClass();

				if (info.Item_Tag.IsValid() == false)
				{
					FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("아이템 태그가 비어있으면 안 됩니다.")));
					return;
				}

				ConsumeTable->AddRow(info.Item_Tag.GetTagName(), info);
				ConsumeTable->MarkPackageDirty();
			}

			if (CreatableItemActorMap[SelectedClassName] == AEnhancementItemActor::StaticClass())
			{
				auto info = Cast<UCommonInfo>(data)->ItemInformation;
				info.ItemActorObject = ItemBlueprint->GeneratedClass.GetDefaultObject()->GetClass();

				if (info.Item_Tag.IsValid() == false)
				{
					FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("아이템 태그가 비어있으면 안 됩니다.")));
					return;
				}

				EnhancementItemTable->AddRow(info.Item_Tag.GetTagName(), info);
				EnhancementItemTable->MarkPackageDirty();
			}


			Cast<AItemActor>(ItemBlueprint->GeneratedClass.GetDefaultObject())->SetItemTag(
				Cast<UItemInformationBase>(data)->GetItemTag());
		}
	}
}

void UItemCreatorWidget::OnClickedCreateButton()
{
	if (DetailsView_Item->GetObject() == nullptr)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("어떤 클래스 타입의 아이템을 생성할지 선택하지 않았습니다.")));
		return;
	}

	if (EditableTextBox_BlueprintName->GetText().IsEmpty() || TextBlock_SelectPath->GetText().IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("경로나 이름이 비었습니다.")));
		return;
	}

	if (auto info = Cast<UItemInformationBase>(DetailsView_Item->GetObject()))
	{
		if (info->GetItemTag().IsValid() == false)
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("아이템 태그가 비어있으면 안 됩니다.")));
			return;
		}

		const auto fullPath = TextBlock_SelectPath->GetText().ToString() + "/" + EditableTextBox_BlueprintName->
			GetText().ToString();
		bool bScc;
		FString msg;
		auto bp = UBlueprintHelperLibrary::CreateBlueprint(fullPath, CreatableItemActorMap[SelectedClassName], bScc,
		                                                   msg);
		if (bScc == false)
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(msg));
		}
		else
		{
			AddTableInfo(bp);
		}
	}
}

void UItemCreatorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UItemCreatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ComboBoxString_Class)
	{
		for (const auto c : CreatableItemActorClass)
		{
			ComboBoxString_Class->AddOption(c->GetName());
			CreatableItemActorMap.Add(c->GetName(), c);
		}

		ComboBoxString_Class->OnSelectionChanged.AddUniqueDynamic(this, &UItemCreatorWidget::OnSelectionChanged);
	}

	if (Button_SelectPath)
	{
		Button_SelectPath->OnClicked.AddUniqueDynamic(this, &UItemCreatorWidget::OnClickedPathButton);
	}

	if (Button_Create)
	{
		Button_Create->OnClicked.AddUniqueDynamic(this, &UItemCreatorWidget::OnClickedCreateButton);
	}
}

/*
void UItemCreatorWidget::HookToolBar()
{
	auto toolMenu = UToolMenus::Get();
	if(auto menu = toolMenu->FindMenu(Menu))
	{
		MenuEntry.Label = Label;
		MenuEntry.ToolTip = ToolTip;
		MenuEntry.Icon = Icon;

		FString str = "KISMETEVENT " +UKismetSystemLibrary::GetObjectName(this)+" OpenWidgetOptions";
		UToolMenuEntryExtensions::SetStringCommand(MenuEntry,EToolMenuStringCommandType::Command,NAME_None,str);
		
		menu->AddMenuEntry(Section,MenuEntry);
		

		toolMenu->RefreshAllWidgets();
	}else
	{
		UE_LOGFMT(LogTemp,Error,"1111111111111111111111111111111");
	}
	
}
*/
