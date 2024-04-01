// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributePreviewWidget.generated.h"


UENUM(BlueprintType)
enum class EAttributePointType : uint8
{
	LEVEL,
	EXP,
	NEXT_EXP,

	MAXHP,
	MAXSP,
	MAXMP,

	STR,
	DEX,
	INT,
	WILL,

	MAX
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddAttributePreviewPoint, EAttributePointType Type);

/**
 * 레벨업 위젯의 왼쪽에 투자 가능한 스텟을 보여주는 위젯입니다.
 */
UCLASS()
class SOULLIKE_API UAttributePreviewWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class ULevelUpWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Cur;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_Next;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock_Next;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Add;

	UPROPERTY(EditAnywhere)
	EAttributePointType AttributePointType;

	UPROPERTY()
	TWeakObjectPtr<class ULevelUpSubsystem> LevelUpSubsystem;

	UPROPERTY(Transient)
	int32 AddPoint = 0;

	virtual void NativeConstruct() override;


	UFUNCTION()
	void OnClickedEvent();

public:
	//미리보는 스텟 포인트가 증가했을 떄 호출되는 이벤트입니다.
	FOnAddAttributePreviewPoint OnAddAttributePreviewPoint;

	//레벨업이 가능할 때, 필요한 설정들을 합니다.
	void ReadyToLevelUp() const;
	void Reset();
	void Init();

	EAttributePointType GetAttributePointType() { return AttributePointType; }
	const int32 GetAddPoint() { return AddPoint; }
};
