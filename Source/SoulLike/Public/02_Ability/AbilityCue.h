// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "AbilityCue.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCue, Log, All);

UENUM(BlueprintType)
enum class EAbilityCueType : uint8
{
	AtLocation,
	AttachToTarget,
	AttachToSocket,
	AtOwnerLocation
};

USTRUCT(BlueprintType)
struct FAbilityCueInformation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag CueTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AAbilityCue> AbilityCueObject;

	//단발성이 아닌 지속성이거나, 자주 재 사용되는 경우 체크하세요.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReusable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAbilityCueType AbilityCueType = EAbilityCueType::AttachToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta=(EditCondition = "AbilityCueType == EAbilityCueType::AttachToSocket"))
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SpawnLocation = FVector::ZeroVector;

	UPROPERTY()
	TWeakObjectPtr<AActor> AttachTarget;

	//일회성으로 사용되는 큐인 경우 거짓을 주세요.
	UPROPERTY()
	bool bIsNotInstanceCue = false;
	bool IsValid() const {return CueTag.IsValid();}
};

UCLASS()
class SOULLIKE_API AAbilityCue : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAbilityCue();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UNiagaraComponent* NiagaraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UAudioComponent* AudioComponent;

	//나이아가라 혹은 케이스케이드 파티클이 끝났을 때 참이 됩니다.
	UPROPERTY(Transient)
	bool bParticleEffectFinish;
	UPROPERTY(Transient)
	bool bAudioFinish;
	UPROPERTY(Transient)
	bool bReusable;

	UPROPERTY()
	FGameplayTag CueTag;


	UFUNCTION()
	void OnNiagaraFinished(UNiagaraComponent* PSystem);
	UFUNCTION()
	void OnAudioFinished();
	UFUNCTION()
	void OnParticleFinished(UParticleSystemComponent* PSystem);


	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnOwnerDeadEvent(AActor* DeadBy);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsReusable() { return bReusable; }

	UFUNCTION(BlueprintCallable)
	void PlayCue(const FAbilityCueInformation& CueInformation);

	UFUNCTION(BlueprintCallable)
	void DeactivateCue() const;

private:
	void ActivateCue() const;
	UFUNCTION()
	void OnDestroyedEvent(AActor* DestroyedActor);

	virtual void Destroyed() override;
};


UCLASS()
class SOULLIKE_API AAbilityCue_SourceActorBased : public AAbilityCue
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};
