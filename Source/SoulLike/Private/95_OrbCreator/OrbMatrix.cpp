// Fill out your copyright notice in the Description page of Project Settings.


#include "95_OrbCreator/OrbMatrix.h"

#include "00_Character/00_Player/PlayerCharacter.h"
#include "00_Character/04_NPC/99_Component/MatrixSlotOpenComponent.h"
#include "93_SaveGame/SoulLikeSaveGame.h"
#include "Logging/StructuredLog.h"


void UOrbMatrix::SaveInfo(const TArray<FMatrixArray>& Matrix, const TArray<FOrbMatrixElementInfo>& SLine,
                          const TArray<FOrbMatrixElementInfo>& DLine, const TArray<FOrbMatrixElementInfo>& ILine,
                          const TArray<FOrbMatrixElementInfo>& FLine)
{
	OrbMatrix = Matrix;

	PhysicalLine = SLine;
	DefenceLine = DLine;
	MagicalLine = ILine;
	FreeLine = FLine;
}

void UOrbMatrix::LoadMatrix(const FOrbMatrixSave& OrbMatrixSave, APlayerCharacter* Player)
{
	if (Player != nullptr)
	{
		OrbMatrix = OrbMatrixSave.WholeMatrix;
		PhysicalLine = OrbMatrixSave.PhysicalLine;
		DefenceLine = OrbMatrixSave.DefenceLine;
		MagicalLine = OrbMatrixSave.MagicalLine;
		FreeLine = OrbMatrixSave.FreeLine;

		//라인이 완성되어있는 상태라면, 완성 보너스를 줍니다.
		if (const auto orbOpenComp = NewObject<UMatrixSlotOpenComponent>(this))
		{
			orbOpenComp->GiveLineCompleteReward(Cast<APlayerController>(Player->GetController()), this,
			                                    EOrbMatrixSlotType::PHYSICAL, false);
			orbOpenComp->GiveLineCompleteReward(Cast<APlayerController>(Player->GetController()), this,
			                                    EOrbMatrixSlotType::MAGICAL, false);
			orbOpenComp->GiveLineCompleteReward(Cast<APlayerController>(Player->GetController()), this,
			                                    EOrbMatrixSlotType::DEFENCE, false);
			orbOpenComp->GiveLineCompleteReward(Cast<APlayerController>(Player->GetController()), this,
			                                    EOrbMatrixSlotType::FREE, false);

			orbOpenComp->ConditionalBeginDestroy();
		}
	}
}

void UOrbMatrix::SetMatrix(const UOrbMatrix* const COD)
{
	AddToRoot();

	OrbMatrix = COD->OrbMatrix;
	PhysicalLine = COD->PhysicalLine;
	DefenceLine = COD->DefenceLine;
	MagicalLine = COD->MagicalLine;
	FreeLine = COD->FreeLine;
}

void UOrbMatrix::BeginDestroy()
{
	if (IsRooted())
	{
		RemoveFromRoot();
	}
	UObject::BeginDestroy();
}
