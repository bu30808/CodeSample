// Fill out your copyright notice in the Description page of Project Settings.


#include "02_Library/BlueprintHelperLibrary.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "KismetCompilerModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/MessageDialog.h"

UBlueprint* UBlueprintHelperLibrary::CreateBlueprint(FString Path, TSubclassOf<UObject> ParentsClass, bool& bSuccess,
                                                     FString& OutMsg)
{
	if (StaticLoadObject(UObject::StaticClass(), nullptr, *Path) != nullptr)
	{
		bSuccess = false;
		OutMsg = TEXT("이미 해당 에셋이 경로에 존재합니다 : ") + Path;
		return nullptr;
	}

	if (!FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentsClass))
	{
		bSuccess = false;
		OutMsg = TEXT("블루프린트로 생성할 수 없는 클래스입니다");
		return nullptr;
	}

	if (UPackage* package = CreatePackage(*Path))
	{
		UClass* bpClass = nullptr;
		UClass* bpGenClass = nullptr;
		FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler").GetBlueprintTypesForClass(
			ParentsClass, bpClass, bpGenClass);

		if (UBlueprint* bp = FKismetEditorUtilities::CreateBlueprint(ParentsClass, package,
		                                                             *FPaths::GetBaseFilename(Path),
		                                                             BPTYPE_Normal, bpClass, bpGenClass))
		{
			FAssetRegistryModule::AssetCreated(bp);
			bp->MarkPackageDirty();
			bSuccess = true;
			return bp;
		}
	}
	else
	{
		bSuccess = false;
		OutMsg = TEXT("경로가 올바르지 않습니다 : ") + Path;
		return nullptr;
	}

	return nullptr;
}

FString UBlueprintHelperLibrary::OpenDirectoryDialog()
{
	// 네이티브 파일 선택 다이얼로그 열기
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		FString DirPath;
		const FString DefaultPath = FPaths::ProjectDir(); // 기본 경로 설정
		auto ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

		// 모달 대화 상자 설정
		FScopedSlowTask SlowTask(0, FText::FromString("선택 경로 대기 중..."));
		SlowTask.MakeDialog();
		
		if (DesktopPlatform->OpenDirectoryDialog(
			ParentWindowHandle, TEXT("생성 경로 선택"), DefaultPath, DirPath))
		{
			const FString ContentString = "/Content";
			const int32 FoundIndex = DirPath.Find(ContentString, ESearchCase::IgnoreCase);
			if (FoundIndex != INDEX_NONE)
			{
				FString ResultString = DirPath.RightChop(FoundIndex + ContentString.Len());
				ResultString.InsertAt(0, "/Game");
				return ResultString;
			}
		}
	}

	return TEXT("경로가 선택되지 않았습니다.");
	/*// Open a native file picker dialog
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		FString DirPath;
		const FString DefaultPath = FPaths::ProjectDir(); // or any other default path
		if (DesktopPlatform->OpenDirectoryDialog(
			nullptr,TEXT("생성 경로 선택"), DefaultPath, DirPath))
		{
			const FString ContentString = "/Content";
			const int32 FoundIndex = DirPath.Find(ContentString, ESearchCase::IgnoreCase);
			if (FoundIndex != INDEX_NONE)
			{
				FString ResultString = DirPath.RightChop(FoundIndex + ContentString.Len());
				//UE_LOG(LogTemp, Warning, TEXT("Result: %s"), *ResultString);
				ResultString.InsertAt(0, "/Game");
				return ResultString;
			}
		}
	}

	return "ERRROR";*/
}

void UBlueprintHelperLibrary::ShowAlertDialog(const FText& DialogText)
{
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

/*
template<typename T>
TMap<FString, TSubclassOf<T>> UBlueprintHelperLibrary::LoadBlueprintClass(UClass* StaticClass)
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

	FName BaseClassName = StaticClass->GetFName();
	FName BaseClassPkgName = StaticClass->GetPackage()->GetFName();
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
	
	TMap<FString,TSubclassOf<T>> OutBlueprintClass;

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
				OutBlueprintClass.Add(blueprintClass->GetName(), blueprintClass);
			}
		}
	}

	return OutBlueprintClass;
}
*/
