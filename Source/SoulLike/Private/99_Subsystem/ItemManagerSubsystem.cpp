// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/ItemManagerSubsystem.h"

#include "Logging/StructuredLog.h"

UItemManagerSubsystem::UItemManagerSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> consumeItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Consume/DT_Consume.DT_Consume'"));
	if (consumeItemTable.Succeeded())
	{
		ConsumeItemTable = consumeItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> spiritItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Spirit/DT_Spirit.DT_Spirit'"));
	if (spiritItemTable.Succeeded())
	{
		SpiritItemTable = spiritItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> armorItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Armor/DT_Armor.DT_Armor'"));
	if (armorItemTable.Succeeded())
	{
		ArmorItemTable = armorItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> ringItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Ring/DT_Ring.DT_Ring'"));
	if (ringItemTable.Succeeded())
	{
		RingItemTable = ringItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> orbCoreItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Orb/Core/DT_OrbCore.DT_OrbCore'"));
	if (orbCoreItemTable.Succeeded())
	{
		OrbCoreItemTable = orbCoreItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> enhancementItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Enhancement/DT_EnhancementItem.DT_EnhancementItem'"));
	if (enhancementItemTable.Succeeded())
	{
		EnhancementItemTable = enhancementItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> potionEnhancementItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Consume/Potion/DT_Potion.DT_Potion'"));
	if (potionEnhancementItemTable.Succeeded())
	{
		PotionItemTable = potionEnhancementItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> orbFragmentItemTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Orb/Fragment/DT_Fragment.DT_Fragment'"));
	if (orbFragmentItemTable.Succeeded())
	{
		OrbFragmentItemTable = orbFragmentItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> keyItemTable(TEXT(
	"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Key/DT_Key.DT_Key'"));
	if (keyItemTable.Succeeded())
	{
		KeyItemTable = keyItemTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> fragmentSlotOpenMaterialTable(TEXT(
	"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Equip/Orb/Fragment/DT_FragmentSlotOpenMaterial.DT_FragmentSlotOpenMaterial'"));
	if (fragmentSlotOpenMaterialTable.Succeeded())
	{
		FragmentSlotOpenMaterialTable = fragmentSlotOpenMaterialTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> stuffItemTable(TEXT(
	"/Script/Engine.DataTable'/Game/Blueprints/04_Item/Stuff/DT_StuffItem.DT_StuffItem'"));
	if (stuffItemTable.Succeeded())
	{
		StuffItemTable = stuffItemTable.Object;
	}
	///Script/Engine.DataTable'/Game/Blueprints/04_Item/Ability/DT_AbilityItem.DT_AbilityItem'

	static ConstructorHelpers::FObjectFinder<UDataTable> abilityItemTable(TEXT(
	"Script/Engine.DataTable'/Game/Blueprints/04_Item/Ability/DT_AbilityItem.DT_AbilityItem'"));
	if (abilityItemTable.Succeeded())
	{
		AbilityItemTable = abilityItemTable.Object;
	}
}


const FItemInformation* UItemManagerSubsystem::GetConsumeItemInformation(FGameplayTag ItemTag) const
{
	if (ConsumeItemTable)
	{
		if (const auto info = ConsumeItemTable->FindRow<FConsumeInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetSpiritItemInformation(FGameplayTag ItemTag) const
{
	if (SpiritItemTable)
	{
		if (const auto info = SpiritItemTable->FindRow<FWeaponInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "무기 테이블을 들고올 수 없습니다.");
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetArmorItemInformation(FGameplayTag ItemTag) const
{
	if (ArmorItemTable)
	{
		if (const auto info = ArmorItemTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetRingItemInformation(FGameplayTag ItemTag) const
{
	if (RingItemTable)
	{
		if (const auto info = RingItemTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetOrbCoreItemInformation(FGameplayTag ItemTag) const
{
	if (OrbCoreItemTable)
	{
		if (const auto info = OrbCoreItemTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}
	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetOrbFragmentItemInformation(FGameplayTag ItemTag) const
{
	if (OrbFragmentItemTable)
	{
		if (const auto info = OrbFragmentItemTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetEnhancementItemInformation(FGameplayTag ItemTag) const
{
	if (EnhancementItemTable)
	{
		if (const auto info = EnhancementItemTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetPotionItemInformation(FGameplayTag ItemTag) const
{
	if (PotionItemTable)
	{
		if (const auto info = PotionItemTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetKeyItemInformation(FGameplayTag ItemTag) const
{
	if (KeyItemTable)
	{
		if (const auto info = KeyItemTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetFragmentSlotOpenMaterialItemInformation(FGameplayTag ItemTag) const
{
	if (FragmentSlotOpenMaterialTable)
	{
		if (const auto info = FragmentSlotOpenMaterialTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FItemInformation* UItemManagerSubsystem::GetStuffItemInformation(FGameplayTag ItemTag) const
{
	if (StuffItemTable)
	{
		if (const auto info = StuffItemTable->FindRow<FItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}

const FAbilityItemInformation* UItemManagerSubsystem::GetAbilityInformation(FGameplayTag ItemTag) const
{
	if (AbilityItemTable)
	{
		if (const auto info = AbilityItemTable->FindRow<FAbilityItemInformation>(ItemTag.GetTagName(), ""))
		{
			return info;
		}
	}

	return nullptr;
}
