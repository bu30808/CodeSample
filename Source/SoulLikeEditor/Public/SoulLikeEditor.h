#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

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
