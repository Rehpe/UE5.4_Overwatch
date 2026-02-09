// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "OWGA_Tracer_Blink.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWGA_Tracer_Blink : public UOWGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGA_Tracer_Blink();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnBlinkFinished();

protected:
	// 점멸 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BlinkDistance;

	// 점멸 지속시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BlinkDuration;

	// 중력 적용 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bEnableGravity;
	
};
