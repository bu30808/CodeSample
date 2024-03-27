// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AbilitySubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSavedAbilityEvent);

UCLASS()
class SOULLIKE_API UAbilitySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//특정 어빌리티에 대한 이벤트를 저장합니다.
	TMap<FGameplayTag, FOnSavedAbilityEvent> EventMap;

	//특정 어빌리티에 대한 이벤트가 있다면 브로드케스트합니다.
	UFUNCTION(BlueprintCallable)
	void TriggerSavedEvent(FGameplayTag AbilityTag);

	//특정 어빌리티에 대한 이벤트를 제거합니다.
	UFUNCTION(BlueprintCallable)
	void RemoveSavedEvent(FGameplayTag AbilityTag);

	//애니메이션 노티파이의 ApplyEffect시 캐릭터별로 저장할 Effect배열입니다.
	TMap<AActor*, TArray<class UAbilityEffect*>> AppliedEffectDuringAnimation;

	/********************************************************/

	//애니메이션 노티파이에서 생성되는 발사체를 관리할 내용들
	TMap<ABaseCharacter*, TArray<class AProjectileActor*>> Projectiles;

	UFUNCTION(BlueprintCallable)
	void AddProjectile(class ABaseCharacter* Owner, class AProjectileActor* Projectile);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AProjectileActor*> GetProjectiles(ABaseCharacter* Owner);

	UFUNCTION(BlueprintCallable)
	void RemoveProjectiles(ABaseCharacter* Owner);
};
