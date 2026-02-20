// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "OWGA_Tracer_Recall.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWGA_Tracer_Recall : public UOWGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGA_Tracer_Recall();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnRecallFinished();

protected:
	UPROPERTY(EditDefaultsOnly)
	float RecallDuration;	// 스킬 소요 시간(0.9)
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> RecallStateEffectClass;		// 무적/타겟불가
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> RecallHpRecoveryEffectClass;	// 조건부 Hp 회복

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> RefillAmmoEffectClass;			// 재장전

private:
	FActiveGameplayEffectHandle ActiveStateEffectHandle;
};
