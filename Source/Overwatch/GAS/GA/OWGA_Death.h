// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "OWGA_Death.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWGA_Death : public UOWGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGA_Death();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DeathEffectClass;
	
protected:
	void StartRagdoll(const FGameplayEventData* TriggerEventData);
};
