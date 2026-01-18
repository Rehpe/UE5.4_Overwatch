// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "OWHeroVoiceData.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWHeroVoiceData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, TObjectPtr<USoundBase>> VoiceMap;
	
	/*UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Death;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Pain;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Jumping;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> JumpEnd;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> MeleeAttack;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> HealPack;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> HealthRestoredFriendly;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Respawn;*/
	
#if WITH_EDITOR
	// 에디터에서만 보임 / 누르면 태그 자동 등록
	UFUNCTION(CallInEditor)
	void FillRequiredTags();
#endif
	
};
