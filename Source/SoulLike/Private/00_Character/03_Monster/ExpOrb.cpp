// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/03_Monster/ExpOrb.h"

#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AExpOrb::AExpOrb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	SphereComponent->SetCollisionProfileName("ExpOrb");

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->HomingAccelerationMagnitude = 2500.f;
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->bIsHomingProjectile = false;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AExpOrb::BeginPlay()
{
	Super::BeginPlay();
}

void AExpOrb::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//모종의 이유로 아직 흡수되지 못했지만, 레벨이 전환되거나 게임이 종료되는 경우, 플레이어 캐릭터를 찾아 경험치를 줍니다.
	/*if (ProjectileMovementComponent->HomingTargetComponent != nullptr)
	{
		OnActorBeginOverlapEvent(this, ProjectileMovementComponent->HomingTargetComponent->GetOwner());
	}*/
}

void AExpOrb::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnActorBeginOverlap.AddUniqueDynamic(this, &AExpOrb::OnActorBeginOverlapEvent);
}

void AExpOrb::OnActorBeginOverlapEvent(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr && OtherActor->IsA<APlayerCharacter>())
	{
		SetActorEnableCollision(false);
		OverlapExplosionCueInfo.AttachTarget = OtherActor;
		Cast<APlayerCharacter>(OtherActor)->GetAbilityComponent()->ApplyCue(OverlapExplosionCueInfo);

		auto attComp = Cast<APlayerCharacter>(OtherActor)->GetAttributeComponent();
		attComp->SetEXP(attComp->GetEXP() + Exp);
		attComp->OnUpdateExp.Broadcast(Exp);
		Destroy();
	}
}

void AExpOrb::Destroyed()
{
	Super::Destroyed();

	GetWorldTimerManager().ClearTimer(HomingResetTimerHandle);
}

void AExpOrb::StartProjectile(ABaseCharacter* Target)
{
	if (Target)
	{
		ProjectileMovementComponent->HomingTargetComponent = Target->GetRootComponent();
		ProjectileMovementComponent->Velocity = GetActorUpVector() * 400.f;
		ProjectileMovementComponent->Activate();

		FTimerHandle hommingTimerHandle;
		FTimerDelegate hommingTimerDel = FTimerDelegate::CreateLambda([&]()
		{
			ProjectileMovementComponent->HomingAccelerationMagnitude = 5000.f;
			ProjectileMovementComponent->Velocity = FVector::ZeroVector;
			ProjectileMovementComponent->bIsHomingProjectile = true;
		});

		GetWorldTimerManager().SetTimer(hommingTimerHandle, hommingTimerDel, 1.f, false);


		FTimerDelegate homingResetTimerDel = FTimerDelegate::CreateLambda([&]()
		{
			if (IsValidLowLevel() == false)
			{
				GetWorldTimerManager().ClearTimer(HomingResetTimerHandle);
				return;
			}

			ProjectileMovementComponent->StopMovementImmediately();
		});

		GetWorldTimerManager().SetTimer(HomingResetTimerHandle, homingResetTimerDel, 2.f, true);
	}
}
