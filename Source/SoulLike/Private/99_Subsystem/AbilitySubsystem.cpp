// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/AbilitySubsystem.h"

#include "02_Ability/02_Projectile/ProjectileActor.h"


void UAbilitySubsystem::AddProjectile(ABaseCharacter* Owner, AProjectileActor* Projectile)
{
	if (Projectiles.Contains(Owner))
	{
		Projectiles[Owner].Projectiles.Emplace(Projectile);
	}
	else
	{
		Projectiles.Add(Owner, FAnimationNotifyProjectiles());
		Projectiles[Owner].Projectiles.Emplace(Projectile);
	}
}

TArray<AProjectileActor*> UAbilitySubsystem::GetProjectiles(ABaseCharacter* Owner)
{
	if (Projectiles.Contains(Owner))
	{
		auto copy = Projectiles[Owner].Projectiles;
		Projectiles[Owner].Projectiles.Empty();
		return copy;
	}

	return TArray<AProjectileActor*>();
}

void UAbilitySubsystem::ClearProjectiles(ABaseCharacter* Owner)
{
	if (Projectiles.Contains(Owner))
	{
		for (auto iter : Projectiles[Owner].Projectiles)
		{
			iter->Destroy();
		}
		Projectiles.Remove(Owner);
	}
}

void UAbilitySubsystem::RemoveProjectile(AProjectileActor* Projectile)
{
	if(Projectile){
		auto owner = Projectile->GetOwner();
		if (Projectiles.Contains(owner))
		{
			Projectiles[owner].Projectiles.Remove(Projectile);
		}
	}
}

void UAbilitySubsystem::OnDestroyedProjectile(AActor* DestroyedActor)
{
	RemoveProjectile(Cast<AProjectileActor>(DestroyedActor));	
}
