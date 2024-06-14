// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "AbilityHelperLibrary.generated.h"

#define MIGHTY_TAG FGameplayTag::RequestGameplayTag("Common.Passive.Mighty")
#define MIGHTY_EFFECT_TAG FGameplayTag::RequestGameplayTag("Common.Passive.Mighty.Effect")
enum class EStatusEffect : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UAbilityHelperLibrary : public UObject
{
	GENERATED_BODY()

public:
	static bool IsAttackAbilityTag(const FGameplayTag& Tag);
	static bool IsNormalAttackAbilityTag(const FGameplayTag& Tag);
	static bool IsStrongAttackAbilityTag(const FGameplayTag& Tag);
	static bool IsDodgeAbilityTag(const FGameplayTag& Tag);
	static bool IsDodgeContAbilityTag(const FGameplayTag& Tag);
	static FGameplayTag GetNormalAttackTag(class UInventoryComponent* InventoryComponent);
	static FGameplayTag GetStrongAttackTag(class UInventoryComponent* InventoryComponent);
	static FGameplayTag GetDodgeTag(UInventoryComponent* InventoryComponent);
	static FGameplayTag GetDodgeContTag(UInventoryComponent* InventoryComponent);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ABaseCharacter* GetTargetFromBlackboard(AActor* AIMonster, FName Key = "Target");
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ABaseCharacter* GetTargetFromBlackboard_AICon(class AAIController* AICon, FName Key = "Target");

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsDead(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UGameInstance* GetGameInstance(class ABaseCharacter* Character);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FGenericTeamId GetTeam(ABaseCharacter* Character);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FGameplayTag GetStatusEffectTag(EStatusEffect StatusEffect);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsMovementBlockedByStatusEffect(class ABaseCharacter* Character);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsStatusEffectActive(class ABaseCharacter* Character, EStatusEffect StatusEffect);

	UFUNCTION(BlueprintCallable)
	static void PlaySound2D(UObject* WorldContext, class USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	static void DeactivateMightyAbility(class ABaseCharacter* Character);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	static class UAttributeComponent* GetAttributeComponent(class ABaseCharacter* Target);


	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsInvincible(class AActor* CheckTarget);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static class UAbilityComponent* GetAbilityComponent(class AActor* CheckTarget);
};
