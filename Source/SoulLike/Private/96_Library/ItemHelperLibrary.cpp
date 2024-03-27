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

#define LOCTEXT_NAMESPACE "ItemToolTipHelper"
FText UItemHelperLibrary::GetItemToolTipText(const FInventoryItem& Item, UInventoryComponent* InventoryComponent)
{
	FText msg = FText::Format(NSLOCTEXT("ItemToolTipHelper", "BaseDescription", "{0}"), Item.GetFormattedDescription());
	if (Item.GetItemInformation()->bSellable)
	{
		msg = FText::Format(NSLOCTEXT("ItemToolTipHelper", "SellablePrice", "{0}\n{1} <price>{2}</>"), msg,
		                    FText::FromString(AttributeTypeToImageString(EAttributeType::EXP)),
		                    FText::AsNumber(Item.GetItemInformation()->Item_Price));
	}
	else
	{
		FText notSellableText = NSLOCTEXT("ItemToolTipHelper", "NotSellableText", "판매불가");
		msg = FText::Format(NSLOCTEXT("ItemToolTipHelper", "NotSellable", "{0}\n<error>{1}</>"), msg, notSellableText);
	}

	if (InventoryComponent->IsEquipped(
		Item.UniqueID))
	{
		FText equippedText = NSLOCTEXT("ItemToolTipHelper", "EquippedText", "장착됨");
		msg = FText::Format(
			NSLOCTEXT("ItemToolTipHelper", "CheckEquipped", "{0}\n<equipped>{1}</>"), msg, equippedText);
	}

	return msg;
}
#undef LOCTEXT_NAMESPACE

#define LOCTEXT_NAMESPACE "FragmentToolTipHelper"

FText UItemHelperLibrary::GetFragmentToolTipText(const FInventoryItem& Item)
{
		
	if (const auto itemInfo = Item.GetItemInformation())
	{
		FFormatOrderedArguments args;
		args.Add(FText::FromString("<orb.name>"+itemInfo->Item_Name.ToString()+"</>)"));// -1
		
		if (IsOrbFragment(Item))
		{
			const auto frag = static_cast<const FOrbFragmentInformation*>(itemInfo);
			
			args.Add(SlotTypeToDecoText(frag->SlotType)); //0
			args.Add(OrbMatrixSlotTypeToText(frag->SlotType));//1
			args.Add(RarityToDecoText(frag->Rarity));//2
			args.Add(RarityToText(frag->Rarity));//3
			
			/*msg += "\n" + SlotTypeToDecoText(frag->SlotType) + SlotTypeToString(frag->SlotType) + "</>";
			msg += "\n" + RarityToDecoText(frag->Rarity);
			msg += RarityToString(frag->Rarity) + "</>\n";*/

			args.Add(Item.GetFormattedDescription());//4
			
			FText additionalAbilityText = NSLOCTEXT("FragmentToolTipHelper", "FragmentAdditionalAbilityText","추가 특성");
			args.Add(FText::FromString(additionalAbilityText.ToString()+"\n"));//5
			
			if (frag->Abilities.Num() > 0)
			{
				FString abilitys = "";
				
				for (auto ab : frag->Abilities)
				{
					abilitys += "<orb.talent>" + ab.GetDefaultObject()->GetAbilityInformation().AbilityName.ToString() + "</>\n";
					abilitys += "<orb.talent.desc>"+ab.GetDefaultObject()->GetAbilityInformation().AbilityDescription.ToString()+ "</>\n";
				}

				args.Add(FText::FromString(abilitys));//6
			}else
			{
				args.Add(FText::FromString("-\n"));//6
			}
			
		
			FText equipmentInfoText = NSLOCTEXT("FragmentToolTipHelper", "equipmentInfoText","장비 정보");
			args.Add(FText::FromString(equipmentInfoText.ToString()+"\n"));//7
			
			if (auto equip = Cast<AEquipmentItemActor>(Item.GetSpawndItemActor()))
			{
				FString equipment = "";
				equipment += equip->GetAttributeDescription().ToString() +"\n";
				equipment += equip->GetEnhancedLevelDescription().ToString()+"\n\n";
				equipment += equip->GetEnhancedAttributeDescription().ToString()+"\n";
				args.Add(FText::FromString(equipment));//8
				
				/*resultMsg += equip->GetAttributeDescription() +"\n";
				resultMsg += equip->GetEnhancedLevelDescription()+"\n\n";
				resultMsg += equip->GetEnhancedAttributeDescription()+"\n";*/
			}else
			{
				args.Add(FText::FromString("-"));//8
			}

			if (Item.GetItemInformation()->bSellable)
			{
				FString sell = "\n" + AttributeTypeToImageString(EAttributeType::EXP) + " <price>" +
					FString::FormatAsNumber(itemInfo->Item_Price) + "</>";
				args.Add(FText::FromString(sell));//9
				/*
				resultMsg += "\n" + AttributeTypeToImageString(EAttributeType::EXP) + " <price>" +
					FString::FormatAsNumber(itemInfo->Item_Price) + "</>";*/
			}
			else
			{
				FText notSellableText =  NSLOCTEXT("FragmentToolTipHelper", "NotSellableText", "판매불가");
				args.Add(FText::FromString("\n<error>"+notSellableText.ToString()+"</>"));//9
				//resultMsg += TEXT("\n<error>판매불가</>");
			}

			if (Cast<ABaseCharacter>(Item.GetSpawndItemActor()->GetOwner())->GetInventoryComponent()->IsEquipped(
				Item.UniqueID))
			{
				FText equippedText =  NSLOCTEXT("FragmentToolTipHelper", "EquippedText", "장착됨");
				args.Add(FText::FromString("\n<equipped>"+equippedText.ToString()+"</>"));//10
				//resultMsg += TEXT("\n<equipped>장착됨</>");
			}else
			{
				args.Add(FText::GetEmpty());//10
			}

			
		}

		/*FString ItemName = TEXT("Magic Sword");
		float ItemPrice = 99.99f;
		FFormatOrderedArguments Args;
		Args.Add(FText::FromString(ItemName));
		Args.Add(FText::AsNumber(ItemPrice));

		FText FormattedText = FText::Format(
			NSLOCTEXT("YourNamespace", "ItemSale", "Item: {0}, Price: ${1}"),
			Args
		);
		*/
		return FText::Format(NSLOCTEXT("FragmentToolTipHelper","FragmentToolTipText","{0}{1}{2}{3}{4}{5}{6}{7}{8}{9}{10}{11}"),args);
	}

	return FText::FromString("ERROR");
}
#undef LOCTEXT_NAMESPACE

#define LOCTEXT_NAMESPACE "ItemDetaulTextHelper"
FText UItemHelperLibrary::GetItemDetailText(const FInventoryItem& Item, UInventoryComponent* InventoryComponent)
{

	FFormatOrderedArguments args;
	
	if (IsEquipment(Item))
	{
		if (auto equip = Cast<AEquipmentItemActor>(Item.GetSpawndItemActor()))
		{
			FString msg;
			msg += equip->GetAttributeDescription().ToString() + "\n";
			msg += equip->GetEnhancedLevelDescription().ToString() + "\n\n";
			msg += equip->GetEnhancedAttributeDescription().ToString() + "\n";

			args.Add(FText::FromString(msg));//0
		}
	}else
	{
		args.Add(FText::FromString(""));//0
	}

	if (IsHPPotion(Item) || IsMPPotion(Item))
	{
		if (auto potion = Cast<APotionItemActor>(Item.GetSpawndItemActor()))
		{
			FString msg = potion->GetEnhancedDescription().ToString() + "\n";
			args.Add(FText::FromString(msg));//1
		}
	}else
	{
		args.Add(FText::FromString(""));//1
	}


	if (Item.GetItemInformation()->bSellable)
	{
		FString msg =  "\n" + AttributeTypeToImageString(EAttributeType::EXP) + " <price>" +
			FString::FormatAsNumber(Item.GetItemInformation()->Item_Price) + "</>";

		args.Add(FText::FromString(msg));//2
	}
	else
	{
		FText defaultStatText =  NSLOCTEXT("ItemDetaulTextHelper", "NotSellableText", "판매불가");
		args.Add(FText::FromString("\n<error>"+defaultStatText.ToString()+"</>"));//2
	}

	if (InventoryComponent->IsEquipped(
		Item.UniqueID))
	{
		FText equippedText =  NSLOCTEXT("ItemDetaulTextHelper", "EquippedText", "장착됨");
		args.Add(FText::FromString("\n<equipped>"+equippedText.ToString()+"</>"));//3
	}else
	{
		args.Add(FText::FromString(""));//3
	}

	return FText::Format(NSLOCTEXT("ItemDetaulTextHelper","ItemDetailText","{0}{1}{2}{3}"),args);
}
#undef LOCTEXT_NAMESPACE

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
	return Item.GetItemInformation()->Item_Tag.MatchesTagExact(
		FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Hp"));
}

bool UItemHelperLibrary::IsMPPotion(const FInventoryItem& Item)
{
	return Item.GetItemInformation()->Item_Tag.MatchesTagExact(
		FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Mp"));
}
#define LOCTEXT_NAMESPACE "ItemTypeToText"
FText UItemHelperLibrary::ItemTypeToText(const FItemInformation* ItemInformation)
{
	switch (ItemInformation->Item_Type)
	{
	case EItemType::NONE:
		break;
	case EItemType::CONSUME:
		return NSLOCTEXT("ItemTypeToText","ConsumeText","소비");
	case EItemType::EQUIPMENT:
		return NSLOCTEXT("ItemTypeToText","EquipmentText","장비");
	case EItemType::ABILITY:
		return NSLOCTEXT("ItemTypeToText","AbilityText","어빌리티");
	case EItemType::ENHANCEMENT:
		return NSLOCTEXT("ItemTypeToText","EnhancmentText","강화소재");
	case EItemType::ETC:
	return NSLOCTEXT("ItemTypeToText","ETCText","기타");
	}

	return FText::FromString("ERROR");
}
#undef LOCTEXT_NAMESPACE
void UItemHelperLibrary::PotionReplenishment(APlayerCharacter* Player)
{
	if (auto invenComp = Player->GetInventoryComponent())
	{
		if (const auto inventoryItem = invenComp->GetItemByTag(
			FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Hp")))
		{
			auto potionInfo = static_cast<const FPotionInformation*>(inventoryItem->GetItemInformation());
			const_cast<FInventoryItem*>(inventoryItem)->ItemCount = potionInfo->Enhancement.CurEnhancement + 1;


			invenComp->OnInventoryWidgetUpdate.Broadcast(inventoryItem->UniqueID, inventoryItem->ItemCount);
			invenComp->OnItemQuickSlotUpdate.Broadcast(inventoryItem->UniqueID,
			                                           inventoryItem->ItemCount);
		}

		if (const auto inventoryItem = invenComp->GetItemByTag(
			FGameplayTag::RequestGameplayTag("Item.Consume.Potion.Mp")))
		{
			auto potionInfo = static_cast<const FPotionInformation*>(inventoryItem->GetItemInformation());
			const_cast<FInventoryItem*>(inventoryItem)->ItemCount = potionInfo->Enhancement.CurEnhancement + 1;
			invenComp->OnInventoryWidgetUpdate.Broadcast(inventoryItem->UniqueID, inventoryItem->ItemCount);
			invenComp->OnItemQuickSlotUpdate.Broadcast(inventoryItem->UniqueID,
			                                           inventoryItem->ItemCount);
		}
	}
}
