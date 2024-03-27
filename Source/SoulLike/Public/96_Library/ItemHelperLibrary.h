// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "03_Widget/01_Menu/00_Inventory/ItemListWidget.h"
#include "UObject/NoExportTypes.h"
#include "ItemHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UItemHelperLibrary : public UObject
{
	GENERATED_BODY()

	static bool CheckEquipmentType(const FEquipmentInformation* EquipInfo, EEquipType CheckType);

public:
	static bool IsEquipment(const FInventoryItem& Info);
	static bool IsOrbCore(const FInventoryItem& Info);
	static bool IsOrbCore(const FGuid& ID, UInventoryComponent* InventoryComponent);
	static bool IsOrbFragment(const FInventoryItem& Info);
	static FText GetItemToolTipText(const FInventoryItem& Item, UInventoryComponent* InventoryComponent);
	static FText GetFragmentToolTipText(const FInventoryItem& Item);
	//아이템 정보 출력 위젯에 세부정보 표시에 사용할 문자열을 만들어 리턴함.
	static FText GetItemDetailText(const FInventoryItem& Item, UInventoryComponent* InventoryComponent);
	//오브 코어 혹은 프래그먼트인지 확인함
	static bool IsOrbItem(const FInventoryItem& Item);
	static bool IsWeapon(const FInventoryItem& Item);
	static bool IsConsume(const FInventoryItem& Item);
	static bool IsSprite(const FInventoryItem& Item);
	static bool IsArmor(const FInventoryItem& Item);
	static bool IsRing(const FInventoryItem& Item);
	static bool IsEnhancement(const FInventoryItem& Item);
	static bool IsHPPotion(const FInventoryItem& Item);
	static bool IsMPPotion(const FInventoryItem& Item);
	static FText ItemTypeToText(const FItemInformation* ItemInformation);

	static void PotionReplenishment(APlayerCharacter* Player);
	
};
