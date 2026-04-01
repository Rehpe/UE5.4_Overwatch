// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OWGameplayAbility.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EOWAbilityActivationPolicy : uint8
{
	// 입력이 들어오면 발동 (기본값)
	OnInputTriggered,

	// 부여되자마자 즉시 발동 (패시브)
	OnSpawn
};

UCLASS()
class OVERWATCH_API UOWGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGameplayAbility();

	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// 어빌리티가 캐릭터에게 주어졌을 때 호출됨 (패시브 발동 처리용)
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintCallable)
	AOWCharacterBase* GetOWCharacter() const;

	// 고유 스킬 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* SkillIcon;
	
	// 스택 제한(기본 1, 스택형 스킬의 경우에 세팅)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxCooldownStacks = 1; // 기본값 1

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag UISlotTag; 

	// 스킬 슬롯에 배치될 순서 (숫자가 낮을수록 왼쪽)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 UISortOrder = 0;
	
protected:
	// 발동 정책
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EOWAbilityActivationPolicy ActivationPolicy;
};
