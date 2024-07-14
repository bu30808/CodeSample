#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Logging/StructuredLog.h"
#include "Modules/ModuleManager.h"



template <typename A>
static void CopyOriginalToTarget(A* Original, A* Target)
{
	if (!Original || !Target)
	{
		return;
	}

	UClass* sourceClass = Original->GetClass();

	for (TFieldIterator<FProperty> iter(sourceClass); iter; ++iter)
	{
		//가져온 UPROPERTY변수
		FProperty* property = *iter;

		//임시변수인 경우 굳이 복사할 필요가 없어요
		if (property->HasAnyPropertyFlags(CPF_Transient))
		{
			continue;
		}

		UE_LOGFMT(LogTemp,Log,"다음 프로퍼티를 복사합니다 : {0}",iter->NamePrivate.ToString());
		auto originalValue = property->ContainerPtrToValuePtr<void>(Original);
		auto targetValue = property->ContainerPtrToValuePtr<void>(Target);

		if (originalValue && targetValue)
		{
			property->CopyCompleteValue(targetValue, originalValue);
		}
	}
}



class FSoulLikeEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	static TSharedPtr<FSlateStyleSet> abilityCueIconInstance;
	static TSharedPtr<FSlateStyleSet> abilityIconInstance;
	static TSharedPtr<FSlateStyleSet> abilityEffectIconInstance;
	static TSharedPtr<FSlateStyleSet> abilityRequirementIconInstance;

	static TSharedPtr<FSlateStyleSet> itemIconInstance;


	//어빌리티 클래스 아이콘 설정
	void SetAbilityIcon();

	//어빌리티 이팩트 아이콘 설정
	void SetAbilityEffectIcon();

	//어빌리티 요구사항 아이콘 설정
	void SetAbilityRequirementIcon();

	//어빌리티 큐 아이콘 설정
	void SetCueIcon();

	//아이템 아이콘
	void SetItemIcon();


	//메뉴에 어빌리티 등록
	void RegisterCreateAbilityRequirementMenuButton();
};
