// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/NoExportTypes.h"
#include "BlueprintHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UBlueprintHelperLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UBlueprint* CreateBlueprint(FString Path, TSubclassOf<UObject> ParentsClass, bool& bSuccess,
	                                   FString& OutMsg);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FString OpenDirectoryDialog();

	UFUNCTION(BlueprintCallable)
	static void ShowAlertDialog(const FText& DialogText);


	template<typename T>
	static TMap<FString, TSubclassOf<T>> LoadBlueprintClass(UClass* StaticClass);
	
};

template <typename T>
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
