// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "OWGA_MeleeAttack.generated.h"

class UOWHeroData;
/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWGA_MeleeAttack : public UOWGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGA_MeleeAttack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	UFUNCTION()
	virtual void OnMontageFinished();

	UFUNCTION()
	void OnMeleeHitEvent(FGameplayEventData Payload);

private:
	bool CheckMeleeHitTrace(FHitResult& OutHitResult);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

private:
	UPROPERTY()
	const UOWHeroData* HeroData;
	
};
