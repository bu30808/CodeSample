// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/01_Component/FootStepComponent.h"

#include "00_Character/BaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Logging/StructuredLog.h"

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


void UFootStepComponent::MakeFootStep(FName SocketName)
{
	if(FootStepDataAsset==nullptr)
	{
		return;
	}
	
	if (Owner.IsValid())
	{
		// 발소리 트리거의 위치와 방향 설정
		FVector startLocation = Owner->GetMesh()->GetSocketLocation(SocketName);
		FVector endLocation = startLocation + (Owner->GetMesh()->GetUpVector() * -50.f); // 발 아래로 라인 트레이스

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Owner.Get());

		FHitResult Hit;
		bool bHit = UKismetSystemLibrary::LineTraceSingle(Owner->GetWorld(),
		                                                  startLocation,
		                                                  endLocation,
		                                                  UEngineTypes::ConvertToTraceType(ECC_Visibility),
		                                                  false,
		                                                  ActorsToIgnore,
		                                                  EDrawDebugTrace::Persistent,
		                                                  Hit, true);
		if (bHit)
		{
			//DrawDebugPoint(GetWorld(),hitResult.Location,50.f,FColor::Blue,true,3.f);
			if (FootStepDataAsset->FootStepSound != nullptr && GetWorld() != nullptr)
			{
				if (Hit.PhysMaterial != nullptr)
				{
					//UE_LOGFMT(LogTemp,Log,"{0}를 밟음 : {1}",Hit.GetActor()->GetName(),StaticEnum<EPhysicalSurface>()->GetValueAsString(Hit.PhysMaterial->SurfaceType));
					//큐 내부에서 switch 합니다. FootStepCue에셋을 참고하세요.
					if (auto sound = UGameplayStatics::SpawnSoundAtLocation(
						GetWorld(), FootStepDataAsset->FootStepSound, Hit.Location))
					{
						sound->SetIntParameter("floor", Hit.PhysMaterial->SurfaceType);
					}
					else
					{
						//UE_LOGFMT(LogTemp, Warning, "사운드 스폰 실패");
					}


					if (FootStepDataAsset->FootStepNiagara.Contains(Hit.PhysMaterial->SurfaceType))
					{
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(
							GetWorld(), FootStepDataAsset->FootStepNiagara[Hit.PhysMaterial->SurfaceType],
							Hit.Location);
					}
				}
				else
				{
					UE_LOGFMT(LogTemp, Warning, "머터리얼이 NULL");
				}
			}
		}
		
		/*TArray<FHitResult> HitResults;
		bool bHit = UKismetSystemLibrary::LineTraceMultiForObjects(
			Owner->GetWorld(),
			startLocation,
			endLocation,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::Persistent,
			HitResults, true
		);
		

		if (bHit)
		{
			//DrawDebugPoint(GetWorld(),hitResult.Location,50.f,FColor::Blue,true,3.f);
			if (FootStepSound != nullptr && GetWorld() != nullptr)
			{
				if (HitResults.IsValidIndex(0) && HitResults[0].PhysMaterial != nullptr)
				{
					//UE_LOGFMT(LogTemp,Log,"{0}를 밟음 : {1}",HitResults[0].GetActor()->GetName(),StaticEnum<EPhysicalSurface>()->GetValueAsString(HitResults[0].PhysMaterial->SurfaceType));
					//큐 내부에서 switch 합니다. FootStepCue에셋을 참고하세요.
					if (auto sound = UGameplayStatics::SpawnSoundAtLocation(
						GetWorld(), FootStepSound, HitResults[0].Location))
					{
						sound->SetIntParameter("floor", HitResults[0].PhysMaterial->SurfaceType);
					}
					else
					{
						//UE_LOGFMT(LogTemp, Warning, "사운드 스폰 실패");
					}


					if (FootStepNiagara.Contains(HitResults[0].PhysMaterial->SurfaceType))
					{
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(
							GetWorld(), FootStepNiagara[HitResults[0].PhysMaterial->SurfaceType],
							HitResults[0].Location);
					}
				}
				else
				{
					UE_LOGFMT(LogTemp, Warning, "머터리얼이 NULL");
				}
			}
		}
		else
		{
			UE_LOGFMT(LogTemp, Warning, "밟은게 없습니다.");
		}*/
	}
}
