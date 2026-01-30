// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "OWGA_Reload.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWGA_Reload : public UOWGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGA_Reload();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	virtual void OnMontageFinished();

	UFUNCTION()
	void OnRefillEvent(FGameplayEventData Payload);

protected:
	// GE_RefillAmmo
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ReloadEffectClass;
};
