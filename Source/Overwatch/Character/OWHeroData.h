// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OWHeroData.generated.h"

class UGameplayEffect;
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
	// 영웅 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HeroName")
	FText HeroName;
	
	// 1p
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|1P")
	TSoftObjectPtr<USkeletalMesh> Mesh1P;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|1P")
	TSoftClassPtr<UAnimInstance> AnimClass1P;

	// 3p
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|3P")
	TSoftObjectPtr<USkeletalMesh> Mesh3P;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|3P")
	TSoftClassPtr<UAnimInstance> AnimClass3P;

	// SkillSet
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSoftObjectPtr<class UOWAbilitySet> AbilitySet;

	// GAS AttributeSet Init
	// 공통
	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "GAS|Init")
	TSubclassOf<UGameplayEffect> InitState_Base;

	// 탄약
	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "GAS|Init")
	TSubclassOf<UGameplayEffect> InitState_Weapon;

	// 스킬/스태미너
	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "GAS|Init")
	TSubclassOf<UGameplayEffect> InitState_Skill;
	
	// Weapon
	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "Equipment")
	TSoftClassPtr<class AOWWeapon> WeaponClass;

	// Voice
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voice")
	TSoftObjectPtr<class UOWHeroVoiceData> VoiceData;

	// SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voice")
	TSoftObjectPtr<class UOWHeroSFXData> SFXData;
	
	// Combat
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float BaseMeleeAttackDamage = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float BaseMeleeAttackRange = 150.0f;
};
