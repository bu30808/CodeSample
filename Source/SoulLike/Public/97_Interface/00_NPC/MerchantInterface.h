// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/04_NPC/99_Component/MerchantComponent.h"
#include "97_Interface/InteractionInterface.h"
#include "UObject/Interface.h"
#include "MerchantInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMerchantInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULLIKE_API IMerchantInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//해당 ID를 가진 아이템을 BuyCount만큼 구입합니다.
	void BuyItemFromPlayer(ANPCBase* Merchant, class APlayerCharacter* PlayerCharacter, FGuid ItemUniqueID,
	                       int32 BuyCount);
	//해당 ID를 가진 아이템을 Count만큼 판매합니다
	void SellItemToPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter, FGuid ItemUniqueID, int32 TradeCount);

	//해당 AbilityTag를 가진 어빌리티를 BuyCount만큼 구입합니다.
	void BuyAbilityFromPlayer(ANPCBase* Merchant, class APlayerCharacter* PlayerCharacter, FMerchandiseAbility
	                          MerchandiseAbility, int32 BuyCount = 1);
	//해당 AbilityTag를 가진 어빌리티를 Count만큼 판매합니다
	void SellAbilityToPlayer(ANPCBase* Merchant, APlayerCharacter* PlayerCharacter,
	                         FMerchandiseAbility MerchandiseAbility, int32 TradeCount = 1);

	UFUNCTION(BlueprintNativeEvent)
	class UMerchantComponent* GetMerchantComponent();
	virtual class UMerchantComponent* GetMerchantComponent_Implementation() const { return nullptr; }

	void UpdateExpWidget(APlayerCharacter* Player, const int32& AddExp);
};
