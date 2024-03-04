// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/03_AbilityTagHandler/AbilityTagHandlerWidget.h"

#include "GameplayTagsManager.h"
#include "00_EditorUtilityWidget/03_AbilityTagHandler/TagRowElementWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxKey.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Misc/MessageDialog.h"

void UAbilityTagHandlerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(ComboBoxString_CommonFilter)
	{
		ComboBoxString_CommonFilter->OnSelectionChanged.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnComboBoxString_CommonFilter);
		AddOptions(ComboBoxString_CommonFilter,CommonFilter);
	}

	if(ComboBoxString_PlayerCharacterFilter)
	{
		ComboBoxString_PlayerCharacterFilter->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxString_PlayerCharacterFilter->OnSelectionChanged.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnComboBoxString_PlayerCharacterFilter);
		AddOptions(ComboBoxString_PlayerCharacterFilter,PlayerCharacterFilter);

	}

	
	if(ComboBoxString_MonsterFilter)
	{
		ComboBoxString_MonsterFilter->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxString_MonsterFilter->OnSelectionChanged.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnComboBoxString_MonsterFilter);
		AddOptions(ComboBoxString_MonsterFilter,MonsterFilter);
		//ComboBoxString_MonsterFilter->SetSelectedOption(TEXT("선택"));
	}


	
	if(ComboBoxString_ItemCommonFilter)
	{
		ComboBoxString_ItemCommonFilter->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxString_ItemCommonFilter->OnSelectionChanged.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnComboBoxString_ItemCommonFilter);
		AddOptions(ComboBoxString_ItemCommonFilter,ItemCommonFilter);
		//ComboBoxString_ItemCommonFilter->SetSelectedOption(TEXT("선택"));
	}

	if(ComboBoxString_ItemConsumeFilter)
	{
		ComboBoxString_ItemConsumeFilter->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxString_ItemConsumeFilter->OnSelectionChanged.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnComboBoxString_ItemConsumeFilter);
		AddOptions(ComboBoxString_ItemConsumeFilter,ItemConsumeFilter);
		//ComboBoxString_ItemConsumeFilter->SetSelectedOption(TEXT("선택"));
	}

	if(ComboBoxString_ItemEquipmentFilter)
	{
		ComboBoxString_ItemEquipmentFilter->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxString_ItemEquipmentFilter->OnSelectionChanged.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnComboBoxString_ItemEquipmentFilter);
		AddOptions(ComboBoxString_ItemEquipmentFilter,ItemEquipmentFilter);
		//ComboBoxString_ItemEquipmentFilter->SetSelectedOption(TEXT("선택"));
	}


	if(ComboBoxString_ItemCoreAndFragmentFilter)
	{
		ComboBoxString_ItemCoreAndFragmentFilter->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxString_ItemCoreAndFragmentFilter->OnSelectionChanged.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnComboBoxString_ItemCoreAndFragmentFilter);
		AddOptions(ComboBoxString_ItemCoreAndFragmentFilter,ItemCoreAndFragmentFilter);
		//ComboBoxString_ItemCoreAndFragmentFilter->SetSelectedOption(TEXT("선택"));
	}

	ComboBoxes = VerticalBox_ComboBox->GetAllChildren();

	if(ScrollBox_TagList)
	{
		ScrollBox_TagList->ClearChildren();
	}

	if(EditableTextBox_NewTag)
	{
		EditableTextBox_NewTag->OnTextCommitted.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnNewTagCommitted);
	}

	if(Button_AddTag)
	{
		Button_AddTag->OnClicked.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnClickedAdd);
	}
}

void UAbilityTagHandlerWidget::OnComboBoxString_CommonFilter(FString SelectedItem, ESelectInfo::Type SelectionType)
{

	TArray<UComboBoxString*> comboBoxArr;
	comboBoxArr.AddUnique(ComboBoxString_CommonFilter);
	ScrollBox_TagList->ClearChildren();
	
	//TArray<FString> CommonFilter = {TEXT("선택"),TEXT("공통"),TEXT("플레이어"),TEXT("몬스터"),TEXT("아이템")};
	if(SelectedItem.Equals(CommonFilter[0]))
	{
		return;
	}
	//공통
	if(SelectedItem.Equals(CommonFilter[1]))
	{
		CreateTagList(TagTableAsset->CommonTags);
		return;
	}
	
	//플레이어
	if(SelectedItem.Equals(CommonFilter[2]))
	{
		comboBoxArr.AddUnique(ComboBoxString_PlayerCharacterFilter);
	}
	//몬스터
	else if(SelectedItem.Equals(CommonFilter[3]))
	{
		comboBoxArr.AddUnique(ComboBoxString_MonsterFilter);
	}
	//아이템
	else if(SelectedItem.Equals(CommonFilter[4]))
	{
		comboBoxArr.AddUnique(ComboBoxString_ItemCommonFilter);
	}
	
	SetVisibleButOtherHidden(comboBoxArr);
}

void UAbilityTagHandlerWidget::OnComboBoxString_PlayerCharacterFilter(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	//	TArray<FString> PlayerCharacterFilter = {TEXT("선택"),TEXT("캐릭터 타입"),TEXT("캐릭터 어빌리티 태그"),TEXT("어빌리티 큐")};
		
	if(SelectedItem.Equals(PlayerCharacterFilter[0]))
	{
		return;
	}

	if(SelectedItem.Equals(PlayerCharacterFilter[1]))
	{
		CreateTagList(TagTableAsset->PlayerCharacterTypeTags);
	}
	else if(SelectedItem.Equals(PlayerCharacterFilter[2]))
	{
		CreateTagList(TagTableAsset->PlayerCharacterAbilityTags);
	
	}
	else if(SelectedItem.Equals(PlayerCharacterFilter[3]))
	{
		CreateTagList(TagTableAsset->PlayerCharacterAbilityCueTags);

	}

}

void UAbilityTagHandlerWidget::OnComboBoxString_MonsterFilter(FString SelectedItem, ESelectInfo::Type SelectionType)
{



	//TArray<FString> MonsterFilter = {TEXT("선택"),TEXT("몬스터 타입 및 어빌리티"),TEXT("몬스터 큐")};
	if(SelectedItem.Equals(MonsterFilter[0]))
	{
		return;
	}

	if(SelectedItem.Equals(MonsterFilter[1]))
	{
		CreateTagList(TagTableAsset->MonsterTags);
	}

	else if(SelectedItem.Equals(MonsterFilter[2]))
	{
		CreateTagList(TagTableAsset->MonsterCueTags);
	}


}




void UAbilityTagHandlerWidget::OnComboBoxString_ItemCommonFilter(FString SelectedItem, ESelectInfo::Type SelectionType)
{

	TArray<UComboBoxString*> comboBoxArr;
	comboBoxArr.AddUnique(ComboBoxString_CommonFilter);
	comboBoxArr.AddUnique(ComboBoxString_ItemCommonFilter);
	
	//TArray<FString> ItemCommonFilter = {TEXT("선택"),TEXT("소비"),TEXT("장비")};
	if(SelectedItem.Equals(ItemCommonFilter[0]))
	{
		return;
	}
	
	if(SelectedItem.Equals(ItemCommonFilter[1]))
	{
		comboBoxArr.AddUnique(ComboBoxString_ItemConsumeFilter);
	}

	else if(SelectedItem.Equals(ItemCommonFilter[2]))
	{
		comboBoxArr.AddUnique(ComboBoxString_ItemEquipmentFilter);
	}
	
	SetVisibleButOtherHidden(comboBoxArr);
}

void UAbilityTagHandlerWidget::OnComboBoxString_ItemConsumeFilter(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	//TArray<FString> ItemConsumeFilter = {TEXT("선택"),TEXT("어빌리티"),TEXT("어빌리티 큐")}
	if(SelectedItem.Equals(ItemConsumeFilter[0]))
	{
		return;
	}
	
	if(SelectedItem.Equals(ItemConsumeFilter[1]))
	{
		CreateTagList(TagTableAsset->ConsumeItemTags);
	}

	else if(SelectedItem.Equals(ItemConsumeFilter[2]))
	{
		CreateTagList(TagTableAsset->ConsumeItemCueTags);
	}
}

void UAbilityTagHandlerWidget::OnComboBoxString_ItemEquipmentFilter(FString SelectedItem,
                                                                    ESelectInfo::Type SelectionType)
{
	TArray<UComboBoxString*> comboBoxArr;
	comboBoxArr.AddUnique(ComboBoxString_CommonFilter);
	comboBoxArr.AddUnique(ComboBoxString_ItemCommonFilter);
	comboBoxArr.AddUnique(ComboBoxString_ItemEquipmentFilter);
	//{TEXT("선택"),TEXT("아이템 태그"),TEXT("어빌리티 큐"),TEXT("코어 및 파편")};
	if(SelectedItem.Equals(ItemEquipmentFilter[0]))
	{
		return;
	}

	if(SelectedItem.Equals(ItemEquipmentFilter[1]))
	{
		CreateTagList(TagTableAsset->EquipmentItemTags);
		SetVisibleButOtherHidden(comboBoxArr);
	}
	else if(SelectedItem.Equals(ItemEquipmentFilter[2]))
	{
		CreateTagList(TagTableAsset->EquipmentItemCueTags);
		SetVisibleButOtherHidden(comboBoxArr);
	}else if(SelectedItem.Equals(ItemEquipmentFilter[3]))
	{
		comboBoxArr.AddUnique(ComboBoxString_ItemCoreAndFragmentFilter);
		SetVisibleButOtherHidden(comboBoxArr);
	}
	
	
}



void UAbilityTagHandlerWidget::OnComboBoxString_ItemCoreAndFragmentFilter(FString SelectedItem,
	ESelectInfo::Type SelectionType)
{
	//{TEXT("선택"),TEXT("코어"),TEXT("파편"),TEXT("파편 큐"),TEXT("라인 완성 보너스")};
	if(SelectedItem.Equals(ItemCoreAndFragmentFilter[0]))
	{
		return;	
	}
	
	if(SelectedItem.Equals(ItemCoreAndFragmentFilter[1]))
	{
		CreateTagList(TagTableAsset->OrbCoreTags);
	}
	else if(SelectedItem.Equals(ItemCoreAndFragmentFilter[2]))
	{
		CreateTagList(TagTableAsset->FragmentTags);
	}
	else if(SelectedItem.Equals(ItemCoreAndFragmentFilter[3]))
	{
		CreateTagList(TagTableAsset->FragmentCueTags);
	}
	else if(SelectedItem.Equals(ItemCoreAndFragmentFilter[4]))
	{
		CreateTagList(TagTableAsset->OrbLineCompleteRewardTags);
	}
}

void UAbilityTagHandlerWidget::OnNewTagCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(CommitMethod==ETextCommit::OnEnter)
	{
		OnClickedAdd();
	}
}

void UAbilityTagHandlerWidget::OnClickedAdd()
{
	if(CurDataTable!=nullptr)
	{
		if(EditableTextBox_NewTag->GetText().IsEmpty() ||  EditableTextBox_NewTagComment->GetText().IsEmpty())
		{
			FMessageDialog::Open(EAppMsgType::Ok,FText::FromString(TEXT("태그 및 설명이 비어있으면 안 됩니다.")));
			return;
		}
		auto newTag = FName(EditableTextBox_NewTag->GetText().ToString());
		auto comment = EditableTextBox_NewTagComment->GetText().ToString();

		CurDataTable->AddRow(newTag,FGameplayTagTableRow(newTag,comment));
		CurDataTable->MarkPackageDirty();
	}else
	{
		FMessageDialog::Open(EAppMsgType::Ok,FText::FromString(TEXT("먼저 카테고리를 선택해야 합니다.")));
	}
}

void UAbilityTagHandlerWidget::AddOptions(UComboBoxString* ComboBox, const TArray<FString>& Filter)
{
	ComboBox->ClearOptions();
	for(auto iter : Filter)
	{
		ComboBox->AddOption(iter);
	}
}

void UAbilityTagHandlerWidget::CreateTagList(UDataTable* DataTable)
{
	CurDataTable = DataTable;
	ScrollBox_TagList->ClearChildren();
	if(DataTable && TagRowElementWidgetObject != nullptr)
	{
		TArray<FGameplayTagTableRow*> out;
		DataTable->GetAllRows<FGameplayTagTableRow>("",out);

		for(auto iter : out)
		{
			if(auto e = CreateWidget<UTagRowElementWidget>(this,TagRowElementWidgetObject))
			{
				e->OnUpdateTag.AddUniqueDynamic(this,&UAbilityTagHandlerWidget::OnRefresh);
				e->SetData(iter, DataTable);
				ScrollBox_TagList->AddChild(e);
			}

			
		}
	}
}

void UAbilityTagHandlerWidget::SetVisibleButOtherHidden(TArray<UComboBoxString*> ComboBoxStringArr)
{
	for(auto iter : ComboBoxes)
	{
		
		iter->SetVisibility(ESlateVisibility::Collapsed);
			
	}
	
	
	for(auto iter : ComboBoxStringArr)
	{
		
		iter->SetVisibility(ESlateVisibility::Visible);
			
	}
}

void UAbilityTagHandlerWidget::OnRefresh(UDataTable* DataTable)
{
	CreateTagList(DataTable);
}

