// Fill out your copyright notice in the Description page of Project Settings.


#include "96_Library/MathHelperLibrary.h"

#include "00_Character/BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"

float UMathHelperLibrary::CalculateDegree(AActor* Target, FVector Point)
{
	/*const auto ownerLocation = Target->GetActorLocation();
	const FVector toPoint = (Point - ownerLocation);
	const FVector ownerForwardVector = Target->GetActorForwardVector();

	// 내적을 사용하여 각도 계산
	const float innerProduct = FVector::DotProduct(toPoint, ownerForwardVector);

	// 내적값을 이용해 각도 계산
	float degree = FMath::Acos(innerProduct);
	degree = FMath::RadiansToDegrees(degree);

	// 외적값을 사용하여 회전 방향 판단
	const FVector outterProduct = FVector::CrossProduct(ownerForwardVector, toPoint);

	// 외적값의 Z 성분을 사용하여 각도의 부호 판단
	if (outterProduct.Z < 0)
	{
		degree = -degree;
	}

	return degree;*/

	const auto ownerLocation = Target->GetActorLocation();
	//타겟과 포인트 사이의 각을 구합니다.
	auto rotation = (Point - ownerLocation);
	//Z축을 날립니다
	rotation *= FVector(1.f, 1.f, 0.f);
	//정규화 합니다.
	rotation.Normalize();


	//타겟의 전방백터
	const auto ownerForwardVector = Target->GetActorForwardVector();
	//각도를 구하기 위한 내적을 합니다 ( 결과로 cos 세타값이 나옴)
	const float innerProduct = FVector::DotProduct(ownerForwardVector, rotation);

	//아크코사인을 이용해서 내적된 결과를 각으로 변환합니다 ( 부호 없는 양수 각도가 나옵니다 )
	const float degree = UKismetMathLibrary::DegAcos(innerProduct);

	//부호 계산을 위해 필요한 외적값을 구합니다.
	const auto outterProduct = FVector::CrossProduct(ownerForwardVector, rotation);

	//Yaw의 부호값을 가져옵니다.
	const float degreeSign = UKismetMathLibrary::SignOfFloat(outterProduct.Z);

	//각도값에 부호값을 곱해 최종값을 구합니다. 이 때 각은 -180 ~ 180 의 값이 됩니다. 정면을 기준으로 좌측이 음의 값, 우측이 양의 값을 가집니다.
	return degree * degreeSign;
}

EDirection UMathHelperLibrary::PointToDirection(AActor* Target, FVector Point, float Front, float Right, float Left)
{
	const auto degree = CalculateDegree(Target, Point);
	Front *= 0.5f;

	//전방
	if (-1 * Front <= degree && degree <= Front)
	{
		return EDirection::Front;
	}

	//우측
	if (Front < degree && degree <= Front + Right)
	{
		return EDirection::Right;
	}

	//좌측
	if (-1 * (Front + Left) <= degree && degree < -1 * Front)
	{
		return EDirection::Left;
	}

	//나머지 후방
	return EDirection::Back;
}

FVector UMathHelperLibrary::GetControllerInputDir(FRotator ControlRotation, FVector MovementInputVector)
{
	const auto& rot = FRotator(0, ControlRotation.Yaw, 0);
	const auto& rightDir = UKismetMathLibrary::GetRightVector(rot) * MovementInputVector.X;
	const auto& forwardDir = UKismetMathLibrary::GetForwardVector(rot) * MovementInputVector.Y;

	return (rightDir + forwardDir).GetSafeNormal();
}

bool UMathHelperLibrary::SameDirection(FVector A_ForwardVector, FVector B_ForwardVector, float Threshold)
{
	/*UE_LOGFMT(LogTemp, Log, "{0}", FVector::DotProduct( A_ForwardVector,B_ForwardVector));
	//Dot 연산의 결과가 음수라면 같은 방향을 바라보는것이 됩니다.
	//결과가 양수인 경우 다른 방향을 바라보고 있다고 할 수 있습니다.
	return FVector::DotProduct(A_ForwardVector, B_ForwardVector) < 0;*/
	UE_LOGFMT(LogTemp, Log, "같은 방향인가요 ? : {0}, {1}", FVector::DotProduct(A_ForwardVector,B_ForwardVector),FVector::DotProduct(A_ForwardVector,B_ForwardVector)>=Threshold);
	return FVector::DotProduct(A_ForwardVector, B_ForwardVector) >= Threshold;
}

EDirection UMathHelperLibrary::DegreeToDirection(float Degree)
{
	float degreePiece = 22.5f;
	
	//전
	if(-1 * degreePiece<Degree&& Degree <= degreePiece)
	{
		return EDirection::Front;
	}

	//전우
	if(degreePiece<Degree&& Degree <= degreePiece * 3)
	{
		return EDirection::FrontRight;
	}
	//우
	if(degreePiece * 3 <Degree&& Degree <= degreePiece * 5)
	{
		return EDirection::Right;
	}
	//후우
	if(degreePiece * 5 <Degree&& Degree <= degreePiece * 7)
	{
		return EDirection::BackRight;
	}
	//후
	if(degreePiece * 5 <Degree&& Degree <= degreePiece * 6)
	{
		return EDirection::Back;
	}
	//전좌
	if(-3 * degreePiece > Degree&& Degree >= -1 * degreePiece)
	{
		return EDirection::FrontLeft;
	}
	//좌
	if(-5 * degreePiece > Degree&& Degree >= -3 * degreePiece)
	{
		return EDirection::Left;
	}
	//좌후
	if(-7 * degreePiece > Degree&& Degree >= -5 * degreePiece)
	{
		return EDirection::BackLeft;
	}
	//후
	return EDirection::Back;
}
