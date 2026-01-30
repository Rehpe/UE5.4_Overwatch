// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OWHeroData.generated.h"

/**
 * 영웅별 Mesh와 AnimBP 정보를 담는 DA
 * SoftObjectPtr로 필요 시에만 로딩함
 */
UCLASS()
class OVERWATCH_API UOWHeroData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("HeroData", GetFName());
	}
	
public:
	// 1p
	UPROPERTY(EditDefaultsOnly, Category = "Visuals|1P")
	TSoftObjectPtr<USkeletalMesh> Mesh1P;

	UPROPERTY(EditDefaultsOnly, Category = "Visuals|1P")
	TSoftClassPtr<UAnimInstance> AnimClass1P;

	// 3p
	UPROPERTY(EditDefaultsOnly, Category = "Visuals|3P")
	TSoftObjectPtr<USkeletalMesh> Mesh3P;

	UPROPERTY(EditDefaultsOnly, Category = "Visuals|3P")
	TSoftClassPtr<UAnimInstance> AnimClass3P;

	// SkillSet
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<class UOWAbilitySet> AbilitySet;

	// GAS AttributeSet Init
	// 공통
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Init")
	TSubclassOf<UGameplayEffect> InitState_Base;

	// 탄약
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Init")
	TSubclassOf<UGameplayEffect> InitState_Weapon;

	// 스킬/스태미너
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Init")
	TSubclassOf<UGameplayEffect> InitState_Skill;
	
	
	// Weapon
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<AActor> WeaponClass;

	// Voice
	UPROPERTY(EditDefaultsOnly, Category = "Voice")
	TObjectPtr<class UOWHeroVoiceData> VoiceData;
};
