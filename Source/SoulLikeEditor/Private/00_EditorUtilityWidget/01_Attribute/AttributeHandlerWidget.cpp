// Fill out your copyright notice in the Description page of Project Settings.


#include "00_EditorUtilityWidget/01_Attribute/AttributeHandlerWidget.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/03_Monster/BaseMonster.h"
#include "02_Library/BlueprintHelperLibrary.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/DetailsView.h"
#include "Components/VerticalBox.h"
#include "Misc/MessageDialog.h"

/*
void UAttributeHandlerWidget::LoadBlueprintClass()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	// The asset registry is populated asynchronously at startup, so there's no guarantee it has finished.
	// This simple approach just runs a synchronous scan on the entire content directory.
	// Better solutions would be to specify only the path to where the relevant blueprints are,
	// or to register a callback with the asset registry to be notified of when it's finished populating.
	TArray< FString > ContentPaths;
	ContentPaths.Add(TEXT("/Game/Blueprints"));
	AssetRegistry.ScanPathsSynchronous(ContentPaths);

	FName BaseClassName = ABaseCharacter::StaticClass()->GetFName();
	FName BaseClassPkgName = ABaseCharacter::StaticClass()->GetPackage()->GetFName();
	FTopLevelAssetPath BaseClassPath(BaseClassPkgName, BaseClassName);

	// Use the asset registry to get the set of all class names deriving from Base
	TSet< FTopLevelAssetPath > DerivedNames;
	
	TArray< FTopLevelAssetPath > BasePaths;
	BasePaths.Add(BaseClassPath);

	TSet< FTopLevelAssetPath > Excluded;
	AssetRegistry.GetDerivedClassNames(BasePaths, Excluded, DerivedNames);
	
	FARFilter Filter;
	FTopLevelAssetPath BPPath(UBlueprint::StaticClass()->GetPathName());
	Filter.ClassPaths.Add(BPPath);
	Filter.bRecursiveClasses = true;
	Filter.PackagePaths.Add(*ContentPaths[0]);
	Filter.bRecursivePaths = true;

	TArray< FAssetData > AssetList;
	AssetRegistry.GetAssets(Filter, AssetList);

	for (auto const& Asset : AssetList)
	{
		// Get the the class this blueprint generates (this is stored as a full path)
		FAssetDataTagMapSharedView::FFindTagResult GeneratedClassPathPtr = Asset.TagsAndValues.FindTag("GeneratedClass");
		{
			if (GeneratedClassPathPtr.IsSet())
			{
				// Convert path to just the name part
				const FString ClassObjectPath = FPackageName::ExportTextPathToObjectPath(GeneratedClassPathPtr.GetValue());
				//const FString ClassName = FPackageName::ObjectPathToObjectName(ClassObjectPath);
				const FTopLevelAssetPath ClassPath = FTopLevelAssetPath(ClassObjectPath);
				

				// Check if this class is in the derived set
				if (!DerivedNames.Contains(ClassPath))
				{
					continue;
				}
				FString N = Asset.GetObjectPathString() + TEXT("_C");
				auto blueprintClass = LoadObject<UClass>(nullptr, *N);
				BaseCharacters.Add(blueprintClass->GetName(), blueprintClass);
				ComboBoxString_AttInit->AddOption(blueprintClass->GetName());
			}
		}
	}
}
*/

void UAttributeHandlerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VerticalBox_AttPerPoint->SetVisibility(ESlateVisibility::Collapsed);
	if (DetailsView_AttPerPoint->GetObject() == nullptr)
	{
		auto data = NewObject<UAttributePerPointHandler>();

		if(AttributePerPointTable)
		{
			TArray<FAttributePerPoint*> out;
			AttributePerPointTable->GetAllRows<FAttributePerPoint>("",out);
			if(out.IsValidIndex(0))
			{
				data->AttributePerPoint = *out[0];
			}
		}
		
		DetailsView_AttPerPoint->SetObject(data);
	}
	Button_AttPerPoint->OnClicked.AddUniqueDynamic(this, &UAttributeHandlerWidget::OnClickedAttPerPoint);


	VerticalBox_AttInit->SetVisibility(ESlateVisibility::Collapsed);
	Button_AttInit->OnClicked.AddUniqueDynamic(this,&UAttributeHandlerWidget::OnClickedAttInit);
	
	BaseCharacters = UBlueprintHelperLibrary::LoadBlueprintClass<ABaseCharacter>(ABaseCharacter::StaticClass());
	for(auto iter : BaseCharacters)
	{
		ComboBoxString_AttInit->AddOption(iter.Key);
	}
	

	ComboBoxString_AttInit->OnSelectionChanged.AddUniqueDynamic(this,&UAttributeHandlerWidget::OnSelectionChanged);

	Button_SaveAttInit->OnClicked.AddUniqueDynamic(this,&UAttributeHandlerWidget::OnClickedSaveAttInit);
	Button_SaveAttPerPoint->OnClicked.AddUniqueDynamic(this,&UAttributeHandlerWidget::OnClickedSaveAttPerPoint);
}

void UAttributeHandlerWidget::OnClickedAttPerPoint()
{
	if (VerticalBox_AttPerPoint)
	{
		if (VerticalBox_AttPerPoint->IsVisible())
		{
			VerticalBox_AttPerPoint->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			VerticalBox_AttPerPoint->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UAttributeHandlerWidget::OnClickedAttInit()
{
	if(VerticalBox_AttInit->IsVisible())
	{
		VerticalBox_AttInit->SetVisibility(ESlateVisibility::Collapsed);
	}else
	{
		VerticalBox_AttInit->SetVisibility(ESlateVisibility::Visible);
	}
}

void UAttributeHandlerWidget::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if(BaseCharacters.Contains(SelectedItem))
	{
		auto target = BaseCharacters[SelectedItem];
		if(auto table = target.GetDefaultObject()->GetAttributeComponent()->AttributeInitTable)
		{
			FGameplayTag tag;
			if(target.GetDefaultObject()->IsA<APlayerCharacter>())
			{
				tag = Cast<APlayerCharacter>(target.GetDefaultObject())->GetCharacterTag();
			}else
			{
				tag = Cast<ABaseMonster>(target.GetDefaultObject())->GetMonsterTag();
			}

			if(!tag.IsValid())
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("가져온 대상의 태그정보가 유효하지 않습니다.")));
				return;
			}
			
	
			auto row = table->FindRow<FAttributeInit>(tag.GetTagName(),"");

			if(DetailsView_AttInit->GetObject() == nullptr)
			{
				if(UAttributeInitHandler* handler = NewObject<UAttributeInitHandler>())
				{
					if(row==nullptr)
					{
						handler->AttributeInit = FAttributeInit();
					}else
					{
						handler->AttributeInit = *row;
					}
					handler->TablePointer = table;
					handler->CharacterPointer = target.GetDefaultObject();
								
					DetailsView_AttInit->SetObject(handler);
				}
			}
			else
			{
				if(auto handler = Cast<UAttributeInitHandler>(DetailsView_AttInit->GetObject()))
				{
					if(row==nullptr)
					{
						handler->AttributeInit = FAttributeInit();
					}else
					{
						handler->AttributeInit = *row;
					}
					handler->TablePointer = table;
					handler->CharacterPointer = target.GetDefaultObject();
				}
			}
			
		}
	}
}

void UAttributeHandlerWidget::OnClickedSaveAttInit()
{
	if(auto handler = Cast<UAttributeInitHandler>(DetailsView_AttInit->GetObject()))
	{
		if(auto table = handler->TablePointer)
		{

			FGameplayTag tag;
			if(handler->CharacterPointer->IsA<APlayerCharacter>())
			{
				tag = Cast<APlayerCharacter>(handler->CharacterPointer)->GetCharacterTag();
			}else
			{
				tag = Cast<ABaseMonster>(handler->CharacterPointer)->GetMonsterTag();
			}
			
			if(!tag.IsValid())
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("가져온 대상의 태그정보가 유효하지 않습니다.")));
				return;
			}
			
			if(auto row = table->FindRow<FAttributeInit>(tag.GetTagName(),""))
			{
				row->Override(handler->AttributeInit);
			}
			else{
				table->AddRow(tag.GetTagName(),handler->AttributeInit);
			}

			if(table->MarkPackageDirty())
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("성공적으로 반영되었습니다. 저장하지 않으면 날아갑니다.")));
			}
		}
	}	
}

void UAttributeHandlerWidget::OnClickedSaveAttPerPoint()
{
	if(AttributePerPointTable)
	{
		if(auto attPerPoint = Cast<UAttributePerPointHandler>(DetailsView_AttPerPoint->GetObject()))
		{
			//ArmorTable->AddRow(info.Item_Tag.GetTagName(), info);
			//ArmorTable->MarkPackageDirty();

			auto RowMap = AttributePerPointTable->GetRowMap();

			// Iterate through the row map and remove all rows
			for (auto& Pair : RowMap)
			{
				uint8* RowData = Pair.Value;
				if (RowData)
				{
					// Mark row for deletion
					AttributePerPointTable->RemoveRow(Pair.Key);
				}
			}
		
			AttributePerPointTable->AddRow("0",attPerPoint->AttributePerPoint);
			if(AttributePerPointTable->MarkPackageDirty())
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("성공적으로 반영되었습니다. 저장하지 않으면 날아갑니다.")));
			}
		}
	}else
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("포인트당 어트리뷰트 테이블이 할당되지 않았습니다.")));
	}
}
