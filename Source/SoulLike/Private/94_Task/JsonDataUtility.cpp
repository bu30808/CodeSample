// Fill out your copyright notice in the Description page of Project Settings.


#include "94_Task/JsonDataUtility.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "Misc/FileHelper.h"


JsonDataUtility::JsonDataUtility()
{
}

JsonDataUtility::~JsonDataUtility()
{
}


bool JsonDataUtility::SaveToJson(const FString& filepath, const FSaveData& data)
{
	FString Jsondata;
	FJsonObjectConverter::UStructToJsonObjectString(data, Jsondata);
	FFileHelper::SaveStringToFile(*Jsondata, *(FPaths::ProjectContentDir() + filepath));
	return true;
}

bool JsonDataUtility::LoadFromJson(const FString& filepath, FSaveData& Data)
{
	FString Jsondata;
	if (!FFileHelper::LoadFileToString(Jsondata, *(FPaths::ProjectContentDir() + filepath)))
	{
		return false;
	}

	return FJsonObjectConverter::JsonObjectStringToUStruct(Jsondata, &Data, 0, 0);
}

/*

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Save;

	UFUNCTION()
	void OnClickedSaveButton();

//Button_Save->OnClicked.AddUniqueDynamic(this, &UCharacterInformationWidget::OnClickedSaveButton);
void UCharacterInformationWidget::OnClickedSaveButton()
{
	UE_LOGFMT(LogTemp, Log, "======== SAVE =========" );
	FSaveData MyFruitData;
	MyFruitData.Name = "";
	// MyFruitData.Size = FVector(1.0f, 1.0f, 1.0f);  // 적절한 크기로 변경
	// MyFruitData.Price = 0;  //
	FString FilePath = "/Json/user.json";
	//JsonDataUtility::SaveToJson(FilePath, MyFruitData );
	JsonDataUtility::LoadFromJson(FilePath, MyFruitData);

	
	UE_LOGFMT(LogTemp, Log, "{0}", MyFruitData.Name);
	UE_LOGFMT(LogTemp, Log, "======== Load =========" );
}
*/
