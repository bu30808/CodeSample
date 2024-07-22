// Fill out your copyright notice in the Description page of Project Settings.


#include "00_Character/01_Component/UROComponent.h"

#include "Logging/StructuredLog.h"


// Sets default values for this component's properties
UUROComponent::UUROComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UUROComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (auto pawn = GetOwner())
	{
		if (m_pLOD_FrameRate.Num() > 0)
		{
			if (UActorComponent* actorComponent = pawn->GetComponentByClass(USkinnedMeshComponent::StaticClass()))
			{
				if (USkinnedMeshComponent* skinnedMeshComponent = Cast<USkinnedMeshComponent>(actorComponent))
				{
					skinnedMeshComponent->bEnableUpdateRateOptimizations = true;
					if(auto param = skinnedMeshComponent->AnimUpdateRateParams)
					{
						param->bShouldUseLodMap = true;
						for (int i = 0; i < m_pLOD_FrameRate.Num(); i++)
						{
							skinnedMeshComponent->AnimUpdateRateParams->LODToFrameSkipMap.Add(i, m_pLOD_FrameRate[i]);
						}
					}
					
				}
			}
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "오너를 가져올 수 없어 URO를 적용할 수 없습니다.");
	}

	/*
	if (m_pLOD_FrameRate.Num() > 0)
	{
		if (UActorComponent* actorComponent = GetOwner()->GetComponentByClass(USkinnedMeshComponent::StaticClass()))
		{
			if(USkinnedMeshComponent* skinnedMeshComponent = Cast<USkinnedMeshComponent>(actorComponent))
			{
				skinnedMeshComponent->bEnableUpdateRateOptimizations = true;
				skinnedMeshComponent->AnimUpdateRateParams->bShouldUseLodMap = true;

				for (int i = 0; i < m_pLOD_FrameRate.Num(); i++)
				{
					skinnedMeshComponent->AnimUpdateRateParams->LODToFrameSkipMap.Add(i, m_pLOD_FrameRate[i]);
				}
			}
		}
	}*/
}
