// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "GameplayTagContainer.h"
#include "02_Ability/AbilityEffect.h"
#include "AbilityEffectHandlerWidget.generated.h"


enum class EAttackType : uint8;
class UAbilityEffect;

UCLASS()
class UAbilityEffectHandler : public UObject
{
	GENERATED_BODY()

public:
	//이 이팩트가 타겟에게 영향을 주어야 할 스테이터스에 대한 정보가 담겨 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttributeEffect> AttributeEffects;
	//이팩트의 오너의 스텟정보에 영향을 받아 적용될 스테이터스에 대한 정보가 담겨 있습니다.
	//절대값을 구조체 내부 Value값으로 설정해주면, 그 값에 스텟정보에 의해 상승되어야 할 값이 더해져 최종값이 결정됩니다.
	//최종값을 결정하는 과정을 UpdateAttributeEffectsAffectedByOwnersAttribute 함수에서 계산합니다.
	//필요하다면 UpdateAttributeEffectsAffectedByOwnersAttribute함수를 블루프린트에서 덮어 쓸 수 있습니다.
	//ex) sp회복량은 의지와 민첩의 영향을 받습니다.
	//절대값 0.1 + 스텟으로 추가 상승되는값 2 => 최종 회복량 2.1
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAttributeEffect> AttributeEffectsAffectedByOwnersAttribute;

	//공격배율을 사용합니까? * 플레이어 캐릭터 전용
	UPROPERTY(EditAnywhere)
	bool bUseChainSystem = false;
	//공격 배율을 사용하기 위한 태그입니다. 없으면 배율이 적용되지 않습니다. * 플레이어 캐릭터 전용
	UPROPERTY(EditAnywhere, meta=(EditCondition ="bUseChainSystem"))
	FGameplayTag ChainTag;

	//이 이팩트가 타격으로 피해를 주기 위한 이팩트일경우 설정하세요
	//이 변수가 참인 경우에는, 무적 태그가 적용되어있는동안 이 이팩트가 적용되지 않는다는 뜻이기도 합니다.
	UPROPERTY(EditAnywhere)
	bool bIsHitDamageEffect;
	UPROPERTY(EditAnywhere, meta=(EditCondition ="bIsHitDamageEffect"))
	EAttackType AttackType;
	//이 이팩트로 주는 피해가 피해 애니메이션을 발동할 수 있는가?
	UPROPERTY(EditAnywhere)
	bool bTriggerHitAnimation = false;
};
/**
 * 
 */
UCLASS()
class SOULLIKEEDITOR_API UAbilityEffectHandlerWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Player;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Monster;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Magic;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Physic;
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBox_Cost;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox_Filter1;
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* VerticalBox_Filter2;
	
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ComboBoxString_Effect;
	UPROPERTY(meta=(BindWidget))
	class UDetailsView* DetailsView_Effect;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Apply;
	
	UPROPERTY()
	TMap<FString,TSubclassOf<class UAbilityEffect>> AbilityEffects;

	UPROPERTY()
	TArray<FString> SearchTag;

	TArray<TTuple<FString,TSubclassOf<class UAbilityEffect>>> CharacterFilteredEffect;

	UPROPERTY()
	TSubclassOf<UAbilityEffect> SelectedEffect;

	virtual void NativeConstruct() override;
	void UpdateOptionWithSearchTag();

	UFUNCTION()
	void OnPlayerCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnMonsterCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnMagicCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnPhysicCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnCostCheckStateChanged(bool bIsChecked);
	UFUNCTION()
	void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnClickedApplyButton();
};
