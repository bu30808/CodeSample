// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/02_Animation/01_Notify/AnimNotify_SpawnProjectile.h"

#include "00_Character/BaseCharacter.h"
#include "02_Ability/02_Projectile/ProjectileActor.h"
#include "99_Subsystem/AbilitySubsystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

class UAbilitySubsystem;

UAnimNotify_SpawnProjectile::UAnimNotify_SpawnProjectile()
{
	bIsNativeBranchingPoint = true;
#if WITH_EDITOR
	bShouldFireInEditor = true;
#endif
}

void UAnimNotify_SpawnProjectile::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);
	if (ProjectileObject)
	{
		if (BranchingPointPayload.SkelMeshComponent)
		{
			if (const auto world = BranchingPointPayload.SkelMeshComponent->GetWorld())
			{
				FActorSpawnParameters spawnParam;

				ABaseCharacter* character = BranchingPointPayload.SkelMeshComponent->GetOwner<ABaseCharacter>();
				if (character != nullptr)
				{
					spawnParam.Instigator = character;
					spawnParam.Owner = character;
				}
				spawnParam.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				const FVector location = BranchingPointPayload.SkelMeshComponent->GetSocketLocation(SocketName);
				const FRotator rotation = BranchingPointPayload.SkelMeshComponent->GetComponentRotation();
				
				if (const auto projectile = world->SpawnActor<AProjectileActor>(
					ProjectileObject, location, rotation, spawnParam))
				{
					if (character != nullptr)
					{
						if (const auto system = UGameplayStatics::GetGameInstance(projectile)->GetSubsystem<
							UAbilitySubsystem>())
						{
							system->AddProjectile(character, projectile);
							projectile->SetEffect(EffectObjects);

							auto projectiles = system->GetProjectiles(character);
							for (const auto p : projectiles)
							{
								if (p->IsValidLowLevel())
								{
									if (bUseBlackboard)
									{
										p->LaunchProjectileWithOption(ProjectileDirection, ProjectileShootType, TimeLimit,nullptr);
									}
									else
									{
										if (character)
										{
											p->LaunchProjectile(character->GetActorForwardVector());
										}
										else
										{
											p->LaunchProjectile(
												BranchingPointPayload.SkelMeshComponent->GetForwardVector());
										}
									}
								}
							}
						}else
						{
							UE_LOGFMT(LogTemp,Error,"UAbilitySubsystem 서브시스템을 가져올 수 없습니다.");
						}
					}
				}
			}
		}
	}else
	{
		UE_LOGFMT(LogTemp,Error,"ProjectileObject is nullptr");
	}
}
