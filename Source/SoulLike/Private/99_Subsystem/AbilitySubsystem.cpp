// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/AbilitySubsystem.h"

#include "Logging/StructuredLog.h"

void UAbilitySubsystem::TriggerSavedEvent(FGameplayTag AbilityTag)
{
	UE_LOGFMT(LogTemp, Log, "TriggerSavedEvent {0} {1}", __FUNCTION__, __LINE__);
	if (EventMap.Contains(AbilityTag))
	{
		EventMap[AbilityTag].Broadcast();
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "TriggerSavedEvent Error {0} : {1} {2}", AbilityTag.ToString(), __FUNCTION__,
		          __LINE__);
	}
}

void UAbilitySubsystem::RemoveSavedEvent(FGameplayTag AbilityTag)
{
	UE_LOGFMT(LogTemp, Log, "RemoveSavedEvent {0} {1}", __FUNCTION__, __LINE__);
	if (EventMap.Contains(AbilityTag))
	{
		EventMap.Remove(AbilityTag);
	}
}

void UAbilitySubsystem::AddProjectile(ABaseCharacter* Owner, AProjectileActor* Projectile)
{
	if (Projectiles.Contains(Owner))
	{
		Projectiles[Owner].Emplace(Projectile);
	}
	else
	{
		Projectiles.Add(Owner, TArray<AProjectileActor*>());
		Projectiles[Owner].Emplace(Projectile);
	}
}

TArray<AProjectileActor*> UAbilitySubsystem::GetProjectiles(ABaseCharacter* Owner)
{
	if (Projectiles.Contains(Owner))
	{
		auto copy = Projectiles[Owner];
		Projectiles[Owner].Empty();
		return copy;
	}

	return TArray<AProjectileActor*>();
}
