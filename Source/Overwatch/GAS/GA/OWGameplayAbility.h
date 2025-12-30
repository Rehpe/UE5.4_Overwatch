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

	// 어빌리티가 캐릭터에게 주어졌을 때 호출됨 (패시브 발동 처리용)
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	// 발동 정책
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Ability Activation")
	EOWAbilityActivationPolicy ActivationPolicy;
};
