// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "OWGA_RegenStamina.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWGA_RegenStamina : public UOWGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGA_RegenStamina();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// GE_Stamina_Regen
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayEffect> RegenEffectClass;
	
	void StartRegenTimer();
	
	UFUNCTION()
	void OnDelayFinished();

};
