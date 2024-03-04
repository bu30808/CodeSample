// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LevelUpSubsystem.generated.h"


enum class EAttributePointType : uint8;
enum class EAttributeType : uint8;

USTRUCT(BlueprintType)
struct FLevelUpData : public FTableRowBase
{
	GENERATED_BODY()

	//다음 레벨로 올라가기 위해 필요한 경험치
	UPROPERTY(EditAnywhere)
	int32 NextExp = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelUp);

/**
 * 
 */
UCLASS()
class SOULLIKE_API ULevelUpSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class UDataTable* LevelUpTable;

public:
	ULevelUpSubsystem();

	//레벨이 올랐을 때 호출되는 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnLevelUp OnLevelUp;


	//현재 레벨에서 타겟레벨까지 올립니다.
	UFUNCTION(BlueprintCallable)
	void TryLevelUp(class UAttributeComponent* AttributeComponent, int32 TargetLevel,
	                const TMap<EAttributePointType, int32> AttributePoints);

	float GetNextLevelUpExp(const int32& CurLevel) const;
	bool CanLevelUp(UAttributeComponent* AttributeComponent);
	//CurLevel+1 레벨로 올릴 수 있는지 확인합니다.
	bool CanLevelUp(UAttributeComponent* AttributeComponent, int32 CurLevel);
	//현 레벨에서 타겟레벨까지 올릴 수 있는지 확인합니다.
	bool CanLevelUp(UAttributeComponent* AttributeComponent, int32 CurLevel, int32 TargetLevel);

	float GetNextLevelUpExp(const int32& CurLevel, const int32& TargetLevel) const;

private:
	bool HasEnoughExp(UAttributeComponent* AttributeComponent, float NextExp);

	//몇포인트 찎었는지 저장한 맵을 가지고 어트리뷰트를 증가시킵니다.
	static void AddAttributePoint(UAttributeComponent* AttributeComponent,
	                              const TMap<EAttributePointType, int32>& AttributePoints);
};
