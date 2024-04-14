// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Item/ItemActor.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "00_Character/BaseCharacter.h"
#include "00_Character/01_Component/AbilityComponent.h"
#include "02_Ability/AbilityBase.h"
#include "96_Library/SaveGameHelperLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


// Sets default values
AItemActor::AItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetRelativeLocation(FVector(0, 0, 0));
	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem> system(TEXT(
			"/Script/Niagara.NiagaraSystem'/Game/Blueprints/04_Item/NS_Item.NS_Item'"));
		if (system.Succeeded())
		{
			NiagaraComponent->SetAsset(system.Object);
		}
	}

	SphereComponent->SetSphereRadius(48.f);
	SphereComponent->SetCollisionProfileName("Item");
	SphereComponent->SetCanEverAffectNavigation(false);
	SphereComponent->CanCharacterStepUpOn = ECB_No;
#if WITH_EDITOR
	RuntimeGrid = "ItemGrid";
#endif
	
}

void AItemActor::DestroyIfPlayerAlreadyGetThisItemFromField()
{
	if(GetOwner()!=UGameplayStatics::GetPlayerCharacter(this,0))
	{
		return;
	}

	
	if (USaveGameHelperLibrary::IsAlreadyPickUppedItem(this))
	{
		Destroy();
	}
	else
	{
		UE_LOGFMT(LogTemp, Log, "다음 필드 아이템은 필드에서 획득된 적이 없습니다 : {0}", GetNameSafe(this));
	}
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	//오너가 플레이어가 아닐 때(필드 아이템) 이미 획득한 상태라면 제거합니다.
	DestroyIfPlayerAlreadyGetThisItemFromField();
}

#if WITH_EDITOR
void AItemActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	NotFormattedDescription = GetNotFormattedDescription();
}


#endif
void AItemActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	NotFormattedDescription = GetNotFormattedDescription();
}

const FItemInformation* AItemActor::GetItemInformation()
{
	UE_LOGFMT(LogTemp, Warning, " AItemActor::GetItemInformation 함수가 오버라이드되지 않았읍니다.");

	return nullptr;
}

FText AItemActor::GetNotFormattedDescription()
{
	if (const auto info = GetItemInformation())
	{
		return info->Item_Description;
	}

	return FText::FromString("NOT VALID ITEM INFO");
}

bool AItemActor::UseItem_Implementation(AActor* Target, const FGuid& ThisItemsUniqueID)
{
	GivenAbility.Empty();
	ItemUniqueID = ThisItemsUniqueID;

	if (const auto info = GetItemInformation())
	{
		for (auto ab : info->Abilities)
		{
			const auto abComp = Cast<ABaseCharacter>(Target)->GetAbilityComponent();
			auto tag = ab.GetDefaultObject()->GetAbilityTag();

			abComp->GiveAbility(ab);

			if (auto givenAbility = abComp->GetAbilityByTag(tag))
			{
				givenAbility->OnOverrideSelfEffect.AddUniqueDynamic(this, &AItemActor::OnOverrideSelfEffectEvent);
				givenAbility->OnOverrideTargetEffect.AddUniqueDynamic(this, &AItemActor::OnOverrideTargetEffectEvent);
				givenAbility->OnEndAbility.AddUniqueDynamic(this, &AItemActor::OnEndAbilityEvent);

				GivenAbility.Add(givenAbility);
			}

			return abComp->ActivateAbility(tag, Target);
		}
	}

	return false;
}

void AItemActor::OnEndAbilityEvent_Implementation()
{
	UE_LOGFMT(LogTemp, Log, "OnEndAbilityEvent : {0}", ItemTag.ToString());

	for (auto ab : GivenAbility)
	{
		UE_LOGFMT(LogTemp, Log, "어빌리티 제거 시도 : {0}", ab->GetAbilityTag().ToString());
		ab->OnOverrideSelfEffect.RemoveAll(this);
		ab->OnOverrideTargetEffect.RemoveAll(this);
		ab->OnEndAbility.RemoveAll(this);

		//등록했던 어빌리티를 컴포넌트에서 제거합니다.
		GetOwner<ABaseCharacter>()->GetAbilityComponent()->RemoveAbility(ab->GetAbilityTag());
	}

	GivenAbility.Empty();
}

void AItemActor::SetItemTag(const FGameplayTag& NewTag)
{
	ItemTag = NewTag;
	NotFormattedDescription = GetNotFormattedDescription();
}

void AItemActor::OnAttachedOwnerDestroyEvent(AActor* DestroyedActor)
{
	//같이 제거됩니다.
	Destroy();
}
