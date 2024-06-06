// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/00_NotifyState/AnimNotifyState_SpawnProjectile.h"

#include "00_Character/BaseCharacter.h"
#include "02_Ability/02_Projectile/ProjectileActor.h"
#include "99_Subsystem/AbilitySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

UAnimNotifyState_SpawnProjectile::UAnimNotifyState_SpawnProjectile()
{
#if WITH_EDITOR
	bShouldFireInEditor = true;
#endif
}


void UAnimNotifyState_SpawnProjectile::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ProjectileObject)
	{
		if (MeshComp)
		{
			if (const auto world = MeshComp->GetWorld())
			{
				FActorSpawnParameters spawnParam;

				ABaseCharacter* character = MeshComp->GetOwner<ABaseCharacter>();
				if (character != nullptr)
				{
					spawnParam.Instigator = character;
					spawnParam.Owner = character;
				}
				spawnParam.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				const FVector location = MeshComp->GetSocketLocation(SocketName);
				const FRotator rotation = FRotationMatrix::MakeFromX(MeshComp->GetForwardVector()).Rotator();

				if (const auto projectile = world->SpawnActor<AProjectileActor>(
					ProjectileObject, location, rotation, spawnParam))
				{
					if (character != nullptr)
					{
						if (const auto system = UGameplayStatics::GetGameInstance(projectile)->GetSubsystem<
							UAbilitySubsystem>())
						{
							system->AddProjectile(character, projectile);
							projectile->AttachToComponent(MeshComp,
							                              FAttachmentTransformRules(AttachmentRule, true), SocketName);
						}
					}
				}
			}
		}
	}
}

void UAnimNotifyState_SpawnProjectile::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (ABaseCharacter* character = MeshComp->GetOwner<ABaseCharacter>())
		{
			if (const auto system = UGameplayStatics::GetGameInstance(character)->GetSubsystem<
				UAbilitySubsystem>())
			{
				auto projectiles = system->GetProjectiles(character);
				for (const auto p : projectiles)
				{
					if (p->IsValidLowLevel())
					{
						p->DetachFromActor(FDetachmentTransformRules(DetachmentRule, true));
						if (bUseBlackboard)
						{
							p->LaunchProjectileWithOption(ProjectileDirection, ProjectileShootType, TimeLimit);
						}
						else
						{
							if (character)
							{
								p->LaunchProjectile(character->GetActorForwardVector());
							}
							else
							{
								p->LaunchProjectile(MeshComp->GetForwardVector());
							}
						}
					}
				}
			}
		}
	}
}
