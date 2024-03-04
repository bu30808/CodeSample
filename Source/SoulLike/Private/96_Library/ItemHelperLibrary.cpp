// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/ItemHelperLibrary.h"

#include "02_Ability/AbilityBase.h"
#include "04_Item/ItemActor.h"
#include "04_Item/00_Consume/00_Potion/PotionItemActor.h"
#include "SoulLike/SoulLike.h"


bool UItemHelperLibrary::CheckEquipmentType(const FEquipmentInformation* EquipInfo, EEquipType CheckType)
{
	return EquipInfo->EquipType == CheckType;
}

bool UItemHelperLibrary::IsEquipment(const FInventoryItem& Info)
{
	if (const auto itemInfo = Info.GetItemInformation())
	{
		if (itemInfo->Item_Type == EItemType::EQUIPMENT)
		{
			return true;
		}
	}

	return false;
}

bool UItemHelperLibrary::IsOrbCore(const FInventoryItem& Info)
{
	if (const auto itemInfo = Info.GetItemInformation())
	{
		if (itemInfo->Item_Type == EItemType::EQUIPMENT)
		{
			if (auto* equip = static_cast<const FEquipmentInformation*>(itemInfo))
			{
				if (equip->EquipType == EEquipType::ORB)
				{
					if (const auto orb = static_cast<const FOrbInformation*>(equip))
					{
						return orb->OrbType == EOrbType::CORE;
					}
				}
			}
		}
	}

	return false;
}

bool UItemHelperLibrary::IsOrbCore(const FGuid& ID, UInventoryComponent* InventoryComponent)
{
	if (InventoryComponent)
	{
		if (InventoryComponent->K2_HasItemByID(ID))
		{
			const auto& item = InventoryComponent->GetInventoryItem(ID);
			return IsOrbCore(item);
		}
	}


	return false;
}

bool UItemHelperLibrary::IsOrbFragment(const FInventoryItem& Info)
{
	if (const auto itemInfo = Info.GetItemInformation())
	{
		if (itemInfo->Item_Type == EItemType::EQUIPMENT)
		{
			if (auto* equip = static_cast<const FEquipmentInformation*>(itemInfo))
			{
				if (equip->EquipType == EEquipType::ORB)
				{
					if (const auto orb = static_cast<const FOrbInformation*>(equip))
					{
						return orb->OrbType == EOrbType::FRAGMENT;
					}
				}
			}
		}
	}

	return false;
}

FString UItemHelperLibrary::GetItemToolTipString(const FInventoryItem& Item, UInventoryComponent* InventoryComponent)
{
	FString msg = Item.GetFormattedDescription();
	
	if (Item.GetItemInformation()->bSellable)
	{
		msg += "\n" + AttributeTypeToImageString(EAttributeType::EXP) + " <price>" +
			FString::FormatAsNumber(Item.GetItemInformation()->Item_Price) + "</>";
	}
	else
	{
		msg += TEXT("\n<error>판매불가</>");
	}

	if (InventoryComponent->IsEquipped(
		Item.UniqueID))
	{
		msg += TEXT("\n<equipped>장착됨</>");
	}

	return msg;
}

FString UItemHelperLibrary::GetFragmentToolTipString(const FInventoryItem& Item)
{

	if(auto itemInfo = Item.GetItemInformation())
	{
		auto msg = "<orb.name>" + itemInfo->Item_Name + "</>";

		if (IsOrbFragment(Item))
		{
			const auto frag = static_cast<const FOrbFragmentInformation*>(itemInfo);
			msg += "\n" + SlotTypeToDecoText(frag->SlotType) + SlotTypeToString(frag->SlotType) + "</>";
			msg += "\n" + RarityToDecoText(frag->Rarity);
			msg += RarityToString(frag->Rarity) + "</>\n";

			msg += "\n" + Item.GetFormattedDescription();
			if (frag->Abilities.Num() > 0)
			{
				msg += TEXT("\n<orb.information>추가 특성</>\n");
				for (auto ab : frag->Abilities)
				{
					msg += "<orb.talent>" + ab.GetDefaultObject()->GetAbilityInformation().AbilityName + "</>\n";
					msg += "<orb.talent.desc>" + ab.GetDefaultObject()->GetAbilityInformation().AbilityDescription +
						"</>\n";
				}
			}
			if(auto equip = Cast<AEquipmentItemActor>(Item.GetSpawndItemActor()))
			{
				msg += equip->GetAttributeDescription() +"\n";
				msg += equip->GetEnhancedLevelDescription()+"\n\n";
				msg += equip->GetEnhancedAttributeDescription()+"\n";
			}

			if (Item.GetItemInformation()->bSellable)
			{
				msg += "\n" + AttributeTypeToImageString(EAttributeType::EXP) + " <price>" +
					FString::FormatAsNumber(itemInfo->Item_Price) + "</>";
			}
			else
			{
				msg += TEXT("\n<error>판매불가</>");
			}

			if (Cast<ABaseCharacter>(Item.GetSpawndItemActor()->GetOwner())->GetInventoryComponent()->IsEquipped(
				Item.UniqueID))
			{
				msg += TEXT("\n<equipped>장착됨</>");
			}
		}

	return msg;
	}

	return "ERROR";
}

FString UItemHelperLibrary::GetItemDetailString(const FInventoryItem& Item, UInventoryComponent* InventoryComponent)
{
	FString msg;
	
	if(IsEquipment(Item))
	{
		if(auto equip = Cast<AEquipmentItemActor>(Item.GetSpawndItemActor()))
		{
			msg += equip->GetAttributeDescription() +"\n";
			msg += equip->GetEnhancedLevelDescription()+"\n\n";
			msg += equip->GetEnhancedAttributeDescription()+"\n";
		}
	}

	if(IsHPPotion(Item) || IsMPPotion(Item))
	{
		if(auto potion = Cast<APotionItemActor>(Item.GetSpawndItemActor()))
		{
			/*msg+= Item.GetFormattedDescription()+"\n\n";*/
			msg+= potion->GetEnhancedDescription()+"\n";
		}
	}
	

	
	if (Item.GetItemInformation()->bSellable)
	{
		msg += "\n" + AttributeTypeToImageString(EAttributeType::EXP) + " <price>" +
			FString::FormatAsNumber(Item.GetItemInformation()->Item_Price) + "</>";
	}
	else
	{
		msg += TEXT("\n<error>판매불가</>");
	}

	if (InventoryComponent->IsEquipped(
		Item.UniqueID))
	{
		msg += TEXT("\n<equipped>장착됨</>");
	}

	return msg;
}

bool UItemHelperLibrary::IsOrbItem(const FInventoryItem& Item)
{
	if (IsOrbCore(Item) || IsOrbFragment(Item))
	{
		return true;
	}

	return false;
}

bool UItemHelperLibrary::IsWeapon(const FInventoryItem& Item)
{
	if (IsEquipment(Item))
	{
		if (const auto itemInfo = Item.GetItemInformation())
		{
			if (const auto equipInfo = static_cast<const FEquipmentInformation*>(itemInfo))
			{
				return CheckEquipmentType(equipInfo, EEquipType::SPIRIT);
			}
		}
	}

	return false;
}

bool UItemHelperLibrary::IsConsume(const FInventoryItem& Item)
{
	return Item.GetItemInformation()->Item_Type == EItemType::CONSUME;
}

bool UItemHelperLibrary::IsSprite(const FInventoryItem& Item)
{
	return IsWeapon(Item);
}

bool UItemHelperLibrary::IsArmor(const FInventoryItem& Item)
{
	if (IsEquipment(Item))
	{
		if (const auto itemInfo = Item.GetItemInformation())
		{
			if (const auto equipInfo = static_cast<const FEquipmentInformation*>(itemInfo))
			{
				return CheckEquipmentType(equipInfo, EEquipType::ARMOR);
			}
		}
	}

	return false;
}

bool UItemHelperLibrary::IsRing(const FInventoryItem& Item)
{
	if (IsEquipment(Item))
	{
		if (const auto itemInfo = Item.GetItemInformation())
		{
			if (const auto equipInfo = static_cast<const FEquipmentInformation*>(itemInfo))
			{
				return CheckEquipmentType(equipInfo, EEquipType::RING);
			}
		}
	}

	return false;
}

bool UItemHelperLibrary::IsEnhancement(const FInventoryItem& Item)
{
	if (const auto itemInfo = Item.GetItemInformation())
	{
		return itemInfo->Item_Type == EItemType::ENHANCEMENT;
	}

	return false;
}

bool UItemHelperLibrary::IsHPPotion(const FInventoryItem& Item)
{
	return Item.GetItemInformation()->Item_Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Hp"));
}

bool UItemHelperLibrary::IsMPPotion(const FInventoryItem& Item)
{
	return Item.GetItemInformation()->Item_Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Mp"));
}

FString UItemHelperLibrary::ItemTypeToString(const FItemInformation* ItemInformation)
{
	switch (ItemInformation->Item_Type) {
	case EItemType::NONE:
		break;
	case EItemType::CONSUME:
		return TEXT("소비");
	case EItemType::EQUIPMENT:
		return TEXT("장비");
	case EItemType::ABILITY:
		return TEXT("어빌리티");
	case EItemType::ENHANCEMENT:
		return TEXT("강화소재");
	case EItemType::ETC:
		return TEXT("기타");
	}

	return "ERROR";
}

void UItemHelperLibrary::PotionReplenishment(APlayerCharacter* Player)
{
	if (auto invenComp = Player->GetInventoryComponent())
	{
		if (const auto inventoryItem = invenComp->GetItemByTag(
			FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Hp")))
		{

			
			auto potionInfo =	static_cast<const FPotionInformation*>(inventoryItem->GetItemInformation());
			const_cast<FInventoryItem*>(inventoryItem)->ItemCount = potionInfo->Enhancement.CurEnhancement + 1;

			
			invenComp->OnInventoryWidgetUpdate.Broadcast(inventoryItem->UniqueID, inventoryItem->ItemCount);
			invenComp->OnItemQuickSlotUpdate.Broadcast(inventoryItem->UniqueID,
													   inventoryItem->ItemCount);
		}
		
		if (const auto inventoryItem = invenComp->GetItemByTag(
			FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Mp")))
		{
			auto potionInfo =	static_cast<const FPotionInformation*>(inventoryItem->GetItemInformation());
			const_cast<FInventoryItem*>(inventoryItem)->ItemCount = potionInfo->Enhancement.CurEnhancement + 1;
			invenComp->OnInventoryWidgetUpdate.Broadcast(inventoryItem->UniqueID, inventoryItem->ItemCount);
			invenComp->OnItemQuickSlotUpdate.Broadcast(inventoryItem->UniqueID,
													   inventoryItem->ItemCount);
		}
	}
}
