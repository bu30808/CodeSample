// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_Ability/AbilityEffect.h"
#include "Blueprint/UserWidget.h"
#include "LevelUpPreviewWidget.generated.h"

enum class EAttributePointType : uint8;
/**
 * 
 */
UCLASS()
class SOULLIKE_API ULevelUpPreviewWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAttributePreviewElementWidget> PreviewElementObject;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox;

	UPROPERTY()
	TMap<EAttributeType, class UAttributePreviewElementWidget*> Elements;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	//투자하는 스텟 포인트 미리보기 위젯에서 투자한 스텟이 하나 증가하면 호출됩니다.
	//아니 이거 왜 자꾸 이 이름으로 함수 만들면 델리게이트 오류 생김? 개빡치네
	UFUNCTION()
	void OnAddAttributePreviewPointEvent(EAttributePointType AttributeType);

	void Reset();
	void Init();
};
