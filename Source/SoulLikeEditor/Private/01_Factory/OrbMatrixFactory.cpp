// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Factory/OrbMatrixFactory.h"

#include "95_OrbCreator/OrbMatrix.h"

UOrbMatrixFactory::UOrbMatrixFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;

	SupportedClass = UOrbMatrix::StaticClass();
}

UObject* UOrbMatrixFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                             UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	auto newObj = NewObject<UOrbMatrix>(InParent, InClass, InName, Flags);
	return newObj;
}

bool UOrbMatrixFactory::ShouldShowInNewMenu() const
{
	return true;
}

FOrbMatrixAssetActions::FOrbMatrixAssetActions(EAssetTypeCategories::Type assetCategory) : AssetCategory(assetCategory)
{
}

FText FOrbMatrixAssetActions::GetName() const
{
	return FText::FromString(TEXT("신력구슬 메트릭스"));
}

UClass* FOrbMatrixAssetActions::GetSupportedClass() const
{
	return UOrbMatrix::StaticClass();
}

uint32 FOrbMatrixAssetActions::GetCategories()
{
	return AssetCategory;
}

FColor FOrbMatrixAssetActions::GetTypeColor() const
{
	return FColor::Cyan;
}
