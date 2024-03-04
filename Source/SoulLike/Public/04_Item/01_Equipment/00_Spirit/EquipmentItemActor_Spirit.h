// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "EquipmentItemActor_Spirit.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AEquipmentItemActor_Spirit : public AEquipmentItemActor
{
	GENERATED_BODY()

protected:
	//어빌리티 끝날 때 호출되는 이벤트에서 어빌리티 등록 해제를 위해 저장하는 변수입니다.
	UPROPERTY()
	TArray<FGameplayTag> GivenActiveAbilityTags;

	//어빌리티 끝날 때 호출되는 이벤트에서 어빌리티 등록 해제를 위해 저장하는 변수입니다.
	UPROPERTY()
	TWeakObjectPtr<class APlayerCharacter> ItemOwner;
	//약, 강공격 태그
	FGameplayTag NormalAttackTag;
	FGameplayTag StrongAttackTag;
	FGameplayTag DodgeTag;
	FGameplayTag DodgeContTag;

	virtual const FItemInformation* GetItemInformation() override;
	virtual bool UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;
	virtual void UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;
	virtual void OnEndAbilityEvent_Implementation() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FGameplayTag& GetNormalAttackTag() const { return NormalAttackTag; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FGameplayTag& GetStrongAttackTag() const { return StrongAttackTag; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FGameplayTag& GetDodgeTag() const { return DodgeTag; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FGameplayTag& GetDodgeContTag() const { return DodgeContTag; }
};
