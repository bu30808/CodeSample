// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "AbilityRequirement.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, DefaultToInstanced)
class SOULLIKE_API UAbilityRequirement : public UObject
{
	GENERATED_BODY()
#if WITH_EDITOR
	friend class UAbilityCreatorWidget_Req;
#endif

protected:
	//이 기술을 사용하기 위해 필요한 코스트 정보. 없으면 코스트 체크에 반드시 성공합니다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="Effect")
	TSubclassOf<class UAbilityEffect> Cost;

	//이 기술에 들어가는 쿨다운, 비워두면 쿨타임이 적용되지 않습니다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="Effect")
	TSubclassOf<class UAbilityEffect> Cooldown;


	/*
	//그 외 사용자 자신에게 적용할 이팩트 정보들
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="Effect")
	TArray<TSubclassOf<class UAbilityEffect>> SelfEffect;

	//그 외 타겟에게 적용할 이팩트 정보들
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="Effect")
	TArray<TSubclassOf<class UAbilityEffect>> TargetEffect;
	*/


	//이 태그가 적용중인 이팩트태그에 전부 있어야지만 해당 어빌리티를 사용할 수 있습니다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="NeedTags")
	FGameplayTagContainer NeedEffectTags;

	//이 태그를 포함한 하위태그가 적용중인 이팩트태그에 하나도 없어야지만 해당 어빌리티를 사용할 수 있습니다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="NotNeedTags")
	FGameplayTagContainer NotNeedEffectTags;


	//해당 태그를 포함한, 하위태그가 어빌리티 목록에 전부 있어야 어빌리티를 사용할 수 있습니다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="NeedTags")
	FGameplayTagContainer NeedActivatedAbilityTags;

	//해당 태그를 포함한, 하위태그가 발동중인 어빌리티목록에 하나도 없어야 어빌리티를 사용할 수 있습니다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), Category="NotNeedTags")
	FGameplayTagContainer NotNeedActivatedAbilityTags;

public:
	/**
	 * @brief 어빌리티를 발동 가능한지 확인합니다.
	 * @param abilityOwner 어빌리티를 사용하는 대상
	 * @return 사용 가능하면 참.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActivateAbility(const FGameplayTag& AbilityTag, class ABaseCharacter* AbilityOwner, UAbilityBase* Ability);


	/**
	 * @brief 어빌리티를 사용하기 위한 코스트가 충분한지 확인합니다.
	 * @param abilityOwner 어빌리티를 사용하는 대상
	 * @return 충분하면 참.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	bool IsCostEnough(class ABaseCharacter* AbilityOwner);
	virtual bool IsCostEnough_Implementation(class ABaseCharacter* AbilityOwner);

protected:
	/**
	 * @brief 이 어빌리티의 쿨다운이 아직 적용중인지 확인합니다.
	 * @param abilityOwner 어빌리티를 사용하는 대상
	 * @return 쿨이 돌고 있으면 참입니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCooldownRunning(class ABaseCharacter* AbilityOwner);

	/**
	 * @brief 어빌리티 사용에 필요한 이팩트 태그가 전부 있는지 확인합니다.
	 * @return 전부 있다면 참입니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAllEffectTagsToNeed(class ABaseCharacter* AbilityOwner);

	/**
	 * @brief 어빌리티 사용을 막는 이팩트 태그가 있는지 확인합니다.
	 * @return 하나라도 있다면 참입니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAnyEffectTagsToNOTNeed(class ABaseCharacter* AbilityOwner);

	/**
	 * @brief 어빌리티 사용에 필요한 '발동중인 어빌리티'태그가 전부 있는지 확인합니다.
	 * @return 전부 있다면 참입니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAllActivatedAbilityTagsToNeed(class ABaseCharacter* AbilityOwner);

	/**
	 * @brief 어빌리티 사용을 막는 '발동중인 어빌리티'태그가 있는지 확인합니다.
	 * @return 하나라도 있다면 참입니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAnyActivatedAbilityTagsToNOTNeed(class ABaseCharacter* AbilityOwner);

private:
	class UAbilityComponent* GetAbilityComponent(ABaseCharacter* AbilityOwner);

public:
	/*const TArray<TSubclassOf<class UAbilityEffect>>& GetSelfEffect() const { return SelfEffect; }
	const TArray<TSubclassOf<class UAbilityEffect>>& GetTargetEffect() const { return TargetEffect; }*/

	const TSubclassOf<UAbilityEffect>& GetCost() const { return Cost; }
	const TSubclassOf<UAbilityEffect>& GetCooldown() const { return Cooldown; }
};
