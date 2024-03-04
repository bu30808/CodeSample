// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Factories/Factory.h"
#include "OrbMatrixFactory.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UOrbMatrixFactory : public UFactory
{
	GENERATED_BODY()

public:
	UOrbMatrixFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

	virtual bool ShouldShowInNewMenu() const override;
};


class FOrbMatrixAssetActions : public FAssetTypeActions_Base
{
public:
	FOrbMatrixAssetActions(EAssetTypeCategories::Type assetCategory = EAssetTypeCategories::Blueprint);

	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override;

private:
	EAssetTypeCategories::Type AssetCategory = EAssetTypeCategories::Blueprint;
};
