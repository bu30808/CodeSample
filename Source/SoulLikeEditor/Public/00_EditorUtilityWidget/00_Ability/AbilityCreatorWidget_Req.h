// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "02_Ability/AbilityEffect.h"
#include "02_Ability/AbilityRequirement.h"
#include "Components/TextBlock.h"
#include "AbilityCreatorWidget_Req.generated.h"

UCLASS(Transient)
class SOULLIKEEDITOR_API UCostCreateInfo : public UObject
{
	GENERATED_BODY()

public:
	//이 이팩트가 적용될 방식을 설정합니다.
	UPROPERTY(EditAnywhere)
	EEffectApplyType EffectApplyType;
	//이 이팩트가 타겟에게 영향을 주어야 할 스테이터스에 대한 정보가 담겨 있습니다.
	UPROPERTY(EditAnywhere)
	TArray<FAttributeEffect> AttributeEffects;
	//이팩트 태그를 설정합니다.
	UPROPERTY(EditAnywhere)
	FGameplayTag EffectTag;
};

UCLASS(Transient)
class SOULLIKEEDITOR_API UCooldownCreateInfo : public UObject
{
	GENERATED_BODY()

public:
	//이 이팩트가 적용될 방식을 설정합니다.
	UPROPERTY(EditAnywhere)
	float DurationTime = 0;
	//이팩트 태그를 설정합니다.
	UPROPERTY(EditAnywhere)
	FGameplayTag EffectTag;
};


UCLASS(Transient)
class SOULLIKEEDITOR_API UReqTagInfo : public UObject
{
	GENERATED_BODY()

public:
	//이 태그가 적용중인 이팩트태그에 전부 있어야지만 해당 어빌리티를 사용할 수 있습니다.
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer NeedEffectTags;

	//이 태그를 포함한 하위태그가 적용중인 이팩트태그에 하나도 없어야지만 해당 어빌리티를 사용할 수 있습니다.
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer NotNeedEffectTags;
};

UCLASS(Transient)
class SOULLIKEEDITOR_API UReqAbilityTagInfo : public UObject
{
	GENERATED_BODY()

public:
	//이 태그가 발동중인 어빌리티 목록에 전부 있어야 어빌리티를 사용할 수 있습니다.
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer NeedActivatedAbilityTags;

	//이 태그 포함한 하위태그가 발동중인 어빌리티목록에 하나도 없어야 어빌리티를 사용할 수 있습니다.
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer NotNeedActivatedAbilityTags;
};


/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UAbilityCreatorWidget_Req : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Dropdown;
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_Req;

	/*UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditableTextBox_BlueprintName;*/

	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Cost;
	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_Cost;

	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Cooldown;
	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_Cooldown;

	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_EffectTag;

	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_AbilityTag;

	UPROPERTY(Transient)
	TObjectPtr<UCostCreateInfo> CostHandler;
	UPROPERTY(Transient)
	TObjectPtr<UCooldownCreateInfo> CooldownHandler;
	UPROPERTY(Transient)
	TObjectPtr<UReqTagInfo> ReqTagHandler;
	UPROPERTY(Transient)
	TObjectPtr<UReqAbilityTagInfo> ReqAbilityTagHandler;

	UFUNCTION()
	void OnClicked();

	UFUNCTION()
	void OnCheckCost(bool bIsChecked);

	UFUNCTION()
	void OnCheckCooldown(bool bIsChecked);

	virtual void NativePreConstruct() override;


	TSubclassOf<UAbilityEffect> CreateCoolDown(const FString& Path, const FString& AbilityName);
	TSubclassOf<UAbilityEffect> CreateCost(const FString& Path, const FString& AbilityName);

	UAbilityRequirement* CreateAR(const FString& Path, const FString& AbilityName);
};
