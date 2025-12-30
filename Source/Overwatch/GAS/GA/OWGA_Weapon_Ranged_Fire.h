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
	
};
