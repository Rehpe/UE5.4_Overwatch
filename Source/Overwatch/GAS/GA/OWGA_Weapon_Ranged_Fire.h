// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "OWGA_Weapon_Ranged_Fire.generated.h"

/**
 * 총 등의 원거리 무기(히트스캔, 투사체) 발사
 */
UCLASS()
class OVERWATCH_API UOWGA_Weapon_Ranged_Fire : public UOWGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGA_Weapon_Ranged_Fire();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,	bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintCallable)
	void FireShot();

protected:
	// 실제로는 AttributeSet의 AttackSpeed를 참조하는 것이 좋지만, 지금은 변수로 처리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Weapon")
	float FireRate = 0.1f;

	// 연사 타이머 핸들
	FTimerHandle FireTimerHandle;
};