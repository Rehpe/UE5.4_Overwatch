// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "OWGCN_Tracer_Recall.generated.h"

struct FOWRecallSnapshot;

UCLASS()
class OVERWATCH_API AOWGCN_Tracer_Recall : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AOWGCN_Tracer_Recall();

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* RecallStartFX_3P; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* RecallEndFX_3P;  

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundBase* RecallSound_1P;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> PP_RecallMaterial;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RecallMID;	// 런타임 수치 조절용 Dynamic Material

private:
	UPROPERTY()
	class AOWCharacterBase* TargetCharacter;

	UPROPERTY()
	class UCameraComponent* TargetCamera;

	TArray<FOWRecallSnapshot> Snapshots;
	
	bool bIsLocallyControlled;
	
	float ElapsedTime;
	float RecallDuration;

	// 기존 카메라
	FVector OriginCameraLocation;
	FRotator OriginCameraRotation;
};
