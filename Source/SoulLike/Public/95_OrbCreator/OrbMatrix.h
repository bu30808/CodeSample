// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OrbMatrix.generated.h"

UENUM(BlueprintType)
enum class EOrbType : uint8
{
	NONE,
	CORE,
	//코어
	FRAGMENT //파편
};

UENUM(BlueprintType)
enum class EOrbMatrixSlotType : uint8
{
	NONE,
	LINE,
	CORE,
	PHYSICAL,
	MAGICAL,
	DEFENCE,
	FREE,
	MAX
};

UENUM(BlueprintType)
enum class EOrbMatrixSlotLineDirection : uint8
{
	None,
	Vertical,
	//↑
	Horizontal,
	//→
	LeftUpToRightDown,
	//↘
	RightUpToLeftDown //↗
};

UENUM(BlueprintType)
enum class EOrbMatrixSlotContent : uint8
{
	Empty,
	Set
};


USTRUCT(BlueprintType)
struct FOrbMatrixElementInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EOrbMatrixSlotType OrbMatrixSlotType = EOrbMatrixSlotType::NONE;

	//슬롯타입이 라인인 경우, 선 방향
	UPROPERTY(EditAnywhere, meta=(EditCondition="OrbMatrixSlotType == EOrbMatrixSlotType::LINE"))
	EOrbMatrixSlotLineDirection SlotLineDirection = EOrbMatrixSlotLineDirection::None;

	UPROPERTY(EditAnywhere)
	EOrbMatrixSlotContent OrbMatrixSlotContent = EOrbMatrixSlotContent::Empty;


	//이 슬롯이 잠겼는가 확인하는 변수입니다.
	UPROPERTY(EditAnywhere,
		meta=(EditCondition =
			"EOrbMatrixSlotType != EOrbMatrixSlotType::CORE && EOrbMatrixSlotType != EOrbMatrixSlotType::LINE"))
	bool bLock = true;

	//파편이 장착되었을 경우, 이 칸에 장착된 파편의 아이디
	UPROPERTY(VisibleAnywhere)
	FGuid UniqueID;

	//행
	UPROPERTY()
	int32 Row = 0;
	//열
	UPROPERTY()
	int32 Col = 0;

	FOrbMatrixElementInfo()
	{
	}

	FOrbMatrixElementInfo(EOrbMatrixSlotType Type, EOrbMatrixSlotLineDirection Dir, EOrbMatrixSlotContent SlotContent,
	                      int32 R, int32 C, bool Lock): OrbMatrixSlotType(Type), SlotLineDirection(Dir),
	                                                    OrbMatrixSlotContent(SlotContent), bLock(Lock), Row(R), Col(C)
	{
	}

	bool operator==(const FOrbMatrixElementInfo& Other) const
	{
		return Row == Other.Row && Col == Other.Col;
	}

	void Exchange(const FOrbMatrixElementInfo& Other)
	{
		OrbMatrixSlotContent = Other.OrbMatrixSlotContent;
		UniqueID = Other.UniqueID;
		Row = Other.Row;
		Col = Other.Col;
	}
};


USTRUCT(BlueprintType)
struct FMatrixArray
{
	GENERATED_USTRUCT_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	TArray<FOrbMatrixElementInfo> horizontal;

public:
	FMatrixArray()
	{
		horizontal.Empty();
	}

	void Add(FOrbMatrixElementInfo value)
	{
		horizontal.Emplace(value);
	}

	FOrbMatrixElementInfo& operator[](int32 index)
	{
		return horizontal[index];
	}


	bool IsValidIndex(int32 index)
	{
		return horizontal.IsValidIndex(index);
	}

	int32 Num()
	{
		return horizontal.Num();
	}

	void Empty()
	{
		horizontal.Empty();
	}
};

//세이브 파일에 사용됩니다.
USTRUCT(BlueprintType)
struct FOrbMatrixSave
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FMatrixArray> WholeMatrix;

	UPROPERTY()
	TArray<FOrbMatrixElementInfo> PhysicalLine;
	UPROPERTY()
	TArray<FOrbMatrixElementInfo> DefenceLine;
	UPROPERTY()
	TArray<FOrbMatrixElementInfo> MagicalLine;
	UPROPERTY()
	TArray<FOrbMatrixElementInfo> FreeLine;

	void Clear()
	{
		WholeMatrix.Empty();
		PhysicalLine.Empty();
		DefenceLine.Empty();
		MagicalLine.Empty();
		FreeLine.Empty();
	}
};

/**
 * 
 */
UCLASS(Blueprintable, DefaultToInstanced)
class SOULLIKE_API UOrbMatrix : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TArray<FMatrixArray> OrbMatrix;


	UPROPERTY(VisibleAnywhere)
	TArray<FOrbMatrixElementInfo> PhysicalLine;
	UPROPERTY(VisibleAnywhere)
	TArray<FOrbMatrixElementInfo> DefenceLine;
	UPROPERTY(VisibleAnywhere)
	TArray<FOrbMatrixElementInfo> MagicalLine;
	UPROPERTY(VisibleAnywhere)
	TArray<FOrbMatrixElementInfo> FreeLine;

	//오브 메트릭스를 저장합니다.
	void SaveInfo(const TArray<FMatrixArray>& Matrix, const TArray<FOrbMatrixElementInfo>& SLine,
	              const TArray<FOrbMatrixElementInfo>& DLine, const TArray<FOrbMatrixElementInfo>& ILine,
	              const TArray<FOrbMatrixElementInfo>& FLine);

	//저장된 오브 메트릭스 정보를 불러옵니다.
	void LoadMatrix(const FOrbMatrixSave& OrbMatrixSave, class APlayerCharacter* Player);

	void SetMatrix(const UOrbMatrix* const COD);

	virtual void BeginDestroy() override;
};
