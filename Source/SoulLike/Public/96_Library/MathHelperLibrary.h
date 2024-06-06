// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MathHelperLibrary.generated.h"

enum class EDirection : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API UMathHelperLibrary : public UObject
{
	GENERATED_BODY()

public:
	//타겟과 포인트의 각도를 구합니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float CalculateDegree(AActor* Target, FVector Point);


	/**
	 * 타겟의 정면을 기준으로 Point좌표를 방향으로 변환합니다.
	 * 전,우,좌 각도를 제외한 나머지 값이 후방 각도가 됩니다.
	 * @param Front 전방각도를 몇도로 설정할것인지 ex) 전방 90도를 정면으로 판정합니다.
	 * @param Right 우측은 몇도로 설정할것인지 ex)우측 120도를 정면으로 판정합니다.
	 * @param Left  우측과 동일
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EDirection PointToDirection(AActor* Target, FVector Point, float Front = 90.f, float Right = 90.f,
	                                   float Left = 90.f);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector GetControllerInputDir(FRotator ControlRotation, FVector MovementInputVector);

	/**
	 * 두개의 전방백터를 이용하여 같은 방향을 바라보고 있는지 확인합니다.
	 * @param A_ForwardVector 
	 * @param B_ForwardVector
	 * @param Threshold = 0.9f; 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool SameDirection(FVector A_ForwardVector, FVector B_ForwardVector, float Threshold = 0.8f);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EDirection DegreeToDirection(float Degree);

};
