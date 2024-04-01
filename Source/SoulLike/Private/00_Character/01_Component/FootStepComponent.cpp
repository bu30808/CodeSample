// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/01_Component/FootStepComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "00_Character/BaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
DEFINE_LOG_CATEGORY(LogFootStep)

#define WATER_TYPE EPhysicalSurface::SurfaceType10

// Sets default values for this component's properties
UFootStepComponent::UFootStepComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UFootStepDataAsset> footstep(TEXT(
		"/Script/SoulLike.FootStepDataAsset'/Game/Blueprints/00_Character/DA_DefaultFootStep.DA_DefaultFootStep'"));
	if (footstep.Succeeded())
	{
		FootStepDataAsset = footstep.Object;
	}


	// ...
}


// Called when the game starts
void UFootStepComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	Owner = GetOwner<ABaseCharacter>();
}


bool UFootStepComponent::CreateFootStepTrace(FName SocketName, FHitResult& OutHit)
{
	if (FootStepDataAsset == nullptr)
	{
		return false;
	}

	if (Owner.IsValid())
	{
		FVector startLocation = Owner->GetMesh()->GetSocketLocation(SocketName) + StartOffset;
		FVector endLocation = Owner->GetMesh()->GetSocketLocation(SocketName) + (Owner->GetMesh()->GetUpVector() *
			TraceLength * -1); // 발 아래로 라인 트레이스

		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(Owner.Get());

		return UKismetSystemLibrary::SphereTraceSingle(this, startLocation, endLocation, 10.f,
		                                               UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
		                                               actorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHit, true,
		                                               FColor::Cyan, FColor::Silver, 0.3f);
	}

	return false;
}

void UFootStepComponent::SpawnSoundAndEffect(const FHitResult& OutHit)
{
	if (OutHit.PhysMaterial != nullptr)
	{
		if (auto sound = UGameplayStatics::SpawnSoundAtLocation(
			GetWorld(), FootStepDataAsset->FootStepSound, OutHit.Location))
		{
			sound->SetIntParameter("floor", OutHit.PhysMaterial->SurfaceType);
		}

		if (FootStepDataAsset->FootStepNiagara.Contains(OutHit.PhysMaterial->SurfaceType))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), FootStepDataAsset->FootStepNiagara[OutHit.PhysMaterial->SurfaceType],
				OutHit.Location);
		}
	}
}

void UFootStepComponent::SpawnWaterSoundAndEffect(const FHitResult& OutHit, const int32& Deep)
{
	if (auto sound = UGameplayStatics::SpawnSoundAtLocation(
		GetWorld(), FootStepDataAsset->FootStepSound, OutHit.Location))
	{
		sound->SetIntParameter("floor", OutHit.PhysMaterial->SurfaceType);
		sound->SetIntParameter("waterDeep", Deep);
	}

	if (FootStepDataAsset->FootStepNiagara.Contains(OutHit.PhysMaterial->SurfaceType))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), FootStepDataAsset->FootStepNiagara[OutHit.PhysMaterial->SurfaceType],
			OutHit.Location);
	}
}

bool UFootStepComponent::CheckWaterDeep(FHitResult& WaterHit)
{
	if (Owner.IsValid())
	{
		FVector startLocation = Owner->GetActorLocation() + StartOffset;
		FVector endLocation = startLocation + (Owner->GetMesh()->GetUpVector() * TraceLength * -300.f); // 발 아래로 라인 트레이스

		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(Owner.Get());

		return UKismetSystemLibrary::SphereTraceSingle(this, startLocation, endLocation, 10.f,
		                                               UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
		                                               actorsToIgnore, EDrawDebugTrace::ForDuration, WaterHit, true,
		                                               FColor::Blue, FColor::Silver, 0.3f);
	}

	return false;
}

void UFootStepComponent::MakeFootStep(FName SocketName)
{
	if (FootStepDataAsset == nullptr)
	{
		return;
	}

	if (Owner.IsValid())
	{
		FHitResult OutHit;
		if (CreateFootStepTrace(SocketName, OutHit))
		{
			if (OutHit.PhysMaterial != nullptr && OutHit.PhysMaterial->SurfaceType == WATER_TYPE)
			{
				//어느정도의 깊이인지 체크합니다.
				FHitResult WaterHit;
				if (CheckWaterDeep(WaterHit))
				{
					//발이 닿은 지점과, 물이 닿은 지점의 거리를 계산합니다.
					float distance =
						FMath::Abs(Owner->GetMesh()->GetSocketLocation(SocketName).Z - WaterHit.Location.Z);
					//거리가 크면, 깊다는 뜻입니다.
					//UE_LOGFMT(LogFootStep,Log,"거리 : {0}",distance);
					//얕음
					if (distance < 10.f)
					{
						//일반적인 적용
						SpawnSoundAndEffect(OutHit);
						return;
					}

					//중간
					if (distance < 50.f)
					{
						SpawnWaterSoundAndEffect(OutHit, 1);
						return;
					}

					//깊음
					SpawnWaterSoundAndEffect(OutHit, 2);
				}
			}
			else
			{
				SpawnSoundAndEffect(OutHit);
			}
		}
	}
}
