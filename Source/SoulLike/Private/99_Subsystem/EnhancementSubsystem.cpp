// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/EnhancementSubsystem.h"

#include "Engine/DataTable.h"
#include "Logging/StructuredLog.h"

UEnhancementSubsystem::UEnhancementSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> enhancementItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Enhancement/DT_EnhancementMaterial.DT_EnhancementMaterial'"));
	if (enhancementItemTable.Succeeded())
	{
		EnhancementMaterialTable = enhancementItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> potionMaterialTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Consume/Potion/DT_PotionEnhancementMaterial.DT_PotionEnhancementMaterial'"));
	if (enhancementItemTable.Succeeded())
	{
		PotionMaterialTable = potionMaterialTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> attValuePerEnhanceTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Enhancement/DT_AttValuePerEnhancement.DT_AttValuePerEnhancement'"));
	if (attValuePerEnhanceTable.Succeeded())
	{
		AttributeValuePerEnhancementTable = attValuePerEnhanceTable.Object;
	}
}

TArray<FEnhancementMaterial*> UEnhancementSubsystem::GetEquipmentEnhancementMaterial() const
{
	TArray<FEnhancementMaterial*> out;
	if (EnhancementMaterialTable)
	{
		EnhancementMaterialTable->GetAllRows("", out);
	}

	return out;
}

TArray<FAttributeValuePerEnhancement*> UEnhancementSubsystem::GetAttributeValuePerEnhancement() const
{
	TArray<FAttributeValuePerEnhancement*> out;
	if (AttributeValuePerEnhancementTable)
	{
		AttributeValuePerEnhancementTable->GetAllRows("", out);
	}

	return out;
}

TArray<FEnhancementMaterial*> UEnhancementSubsystem::GetPotionEnhancementMaterial() const
{
	TArray<FEnhancementMaterial*> out;
	if (PotionMaterialTable)
	{
		PotionMaterialTable->GetAllRows("", out);
	}

	return out;
}

float UEnhancementSubsystem::GetAttributeValuePerEnhancement(EAttributeType Key)
{
	if (AttributeValuePerEnhancementTable)
	{
		const FString enumToString = StaticEnum<EAttributeType>()->GetValueAsString(Key);
		if (auto data = AttributeValuePerEnhancementTable->FindRow<FAttributeValuePerEnhancement>(
			FName(*enumToString), ""))
		{
			return data->AddValue;
		}
	}

	UE_LOGFMT(LogTemp, Error, "{0} {1}", __FUNCTION__, __LINE__);

	return 0;
}
