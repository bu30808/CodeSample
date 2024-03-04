// Fill out your copyright notice in the Description page of Project Settings.


#include "99_Subsystem/LevelUpSubsystem.h"

#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AttributeComponent.h"
#include "00_Character/01_Component/InventoryComponent.h"
#include "04_Item/ItemActor.h"
#include "04_Item/01_Equipment/EquipmentItemActor.h"
#include "Logging/StructuredLog.h"

ULevelUpSubsystem::ULevelUpSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> levelUpTable(TEXT(
		"/Script/Engine.DataTable'/Game/Blueprints/00_Character/03_Attribute/DT_LevelUp.DT_LevelUp'"));
	if (levelUpTable.Succeeded())
	{
		LevelUpTable = levelUpTable.Object;
	}
}

void ULevelUpSubsystem::TryLevelUp(UAttributeComponent* AttributeComponent, int32 TargetLevel,
                                   const TMap<EAttributePointType, int32> AttributePoints)
{
	if (AttributeComponent)
	{
		const int32& curLevel = AttributeComponent->GetLevel();
		const auto needExp = GetNextLevelUpExp(curLevel, TargetLevel);
		if (needExp != -1)
		{
			if (HasEnoughExp(AttributeComponent, needExp))
			{
				//경험치 차감
				AttributeComponent->SetEXP(AttributeComponent->GetEXP() - needExp);
				AddAttributePoint(AttributeComponent, AttributePoints);
				OnLevelUp.Broadcast();
			}
		}
	}
}

float ULevelUpSubsystem::GetNextLevelUpExp(const int32& CurLevel) const
{
	if (LevelUpTable)
	{
		TArray<FLevelUpData*> out;
		LevelUpTable->GetAllRows<FLevelUpData>("", out);

		return out[CurLevel]->NextExp;
	}

	return -1;
}

bool ULevelUpSubsystem::CanLevelUp(UAttributeComponent* AttributeComponent)
{
	const int32& curLevel = AttributeComponent->GetLevel();
	const auto nextExp = GetNextLevelUpExp(curLevel);

	return HasEnoughExp(AttributeComponent, nextExp);
}

bool ULevelUpSubsystem::CanLevelUp(UAttributeComponent* AttributeComponent, int32 CurLevel)
{
	const auto nextExp = GetNextLevelUpExp(CurLevel);

	return HasEnoughExp(AttributeComponent, nextExp);
}

bool ULevelUpSubsystem::CanLevelUp(UAttributeComponent* AttributeComponent, int32 CurLevel, int32 TargetLevel)
{
	if (CurLevel > TargetLevel)
	{
		return false;
	}


	float sum = 0;
	for (auto i = CurLevel; i < TargetLevel; i++)
	{
		sum += GetNextLevelUpExp(i);

		if (!HasEnoughExp(AttributeComponent, sum))
		{
			return false;
		}
	}

	return true;
}

float ULevelUpSubsystem::GetNextLevelUpExp(const int32& CurLevel, const int32& TargetLevel) const
{
	if (CurLevel > TargetLevel)
	{
		return -1;
	}


	float sum = 0;
	for (auto i = CurLevel; i < TargetLevel; i++)
	{
		sum += GetNextLevelUpExp(i);
	}

	return sum;
}

bool ULevelUpSubsystem::HasEnoughExp(UAttributeComponent* AttributeComponent, float NextExp)
{
	if (NextExp == -1)
	{
		return false;
	}

	if (AttributeComponent)
	{
		return AttributeComponent->GetEXP() >= NextExp;
	}
	return false;
}

void ULevelUpSubsystem::AddAttributePoint(UAttributeComponent* AttributeComponent,
                                          const TMap<EAttributePointType, int32>& AttributePoints)
{
	for (auto iter : AttributePoints)
	{
		AttributeComponent->AddLevelUpPoint(iter.Key, iter.Value);
	}
}
