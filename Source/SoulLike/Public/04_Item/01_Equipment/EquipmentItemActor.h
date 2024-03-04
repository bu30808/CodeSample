// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Item/ItemActor.h"
#include "EquipmentItemActor.generated.h"

enum class EAttributeApplyMethod : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API AEquipmentItemActor : public AItemActor
{
	GENERATED_BODY()


	friend class UGameLoadHandler;
	
protected:
	//장비의 강화 수치를 저장할 변수입니다.
	UPROPERTY(VisibleAnywhere)
	FEnhancement Enhancement;


	virtual void PostInitializeComponents() override;
public:
	virtual FString GetEnhancedLevelDescription();
	virtual FString GetAttributeDescription();
	virtual FString GetEnhancedAttributeDescription();
protected:
	virtual FString GetFormattedDescription_Implementation() override;
	virtual void
	OnOverrideSelfEffectEvent_Implementation(const TArray<UAbilityEffect*>& SelfEffect, AActor* Target) override;
	virtual void UnEquip_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;
	virtual bool UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID) override;

	void ApplyEnhance(AActor* Target, EAttributeApplyMethod Method);
	void UpdateCharacterInfo(AActor* Target);

public:
	//이 장비가 가진 어빌리티를 강제종료시킵니다.
	void ForceEndAbility(AActor* Target);
	//이 장비가 가진 어빌리티를 강제적용합니다.
	void ForceApplyAbility(AActor* Target);
	//장비의 강화수치를 한단계 올립니다.
	void IncreaseEnhance(EAttributeType IncreaseAttribute);

	const FEnhancement& GetEnhancementInfo() { return Enhancement; }
};
