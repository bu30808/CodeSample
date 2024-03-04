// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "WidgetManagerComponent.generated.h"


/**
 * 위젯을 저장하기 위한 데이터테이블 정의
 */
USTRUCT(BlueprintType)
struct FWidgetTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	/**
	 * 위젯의 소프트 포인터
	 */
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class UUserWidget> WidgetSoftClass;
};

/**
 * 위젯을 생성하고 관리하는 컴포넌트 입니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UWidgetManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWidgetManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	 * 위젯의 정보가 담긴 데이터테이블
	 */
	UPROPERTY(EditAnywhere)
	class UDataTable* WidgetTable;


	/**
	 * AddWidget에서 생성된 위젯이 저장됩니다.
	 */
	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, class UUserWidget*> Widgets;

	/**
	 * 데이터테이블에서 위젯 클래스를 가져옵니다.
	 * @param WidgetTag 찾을 태그
	 * @return 찾은 위젯 오브젝트 클래스
	 */
	UClass* GetWidgetClassFromTable(FGameplayTag WidgetTag);

public:
	/*
	 * 전달된 태그에 해당하는 위젯을 생성하고, 화면에 부착합니다.
	 * @param WidgetTag 찾을 위젯의 태그
	 * @param ZOrder 위젯의 정렬 순서, 값이 높으면 낮은 값들의 위젯을 가리게 됩니다.
	 * @param bSave 참이면 Widgets변수에 저장합니다.
	 */
	UFUNCTION(BlueprintCallable)
	UUserWidget* AddWidget(FGameplayTag WidgetTag, int32 ZOrder = 0, bool bSave = true);

	/**
	 * 해당 태그의 위젯을 Widgets에서 찾아 가져옵니다.
	 * @param WidgetTag 찾을 위젯
	 * @return 찾은 위젯 포인터
	 */
	UFUNCTION(BlueprintCallable)
	UUserWidget* GetWidget(FGameplayTag WidgetTag);
};
