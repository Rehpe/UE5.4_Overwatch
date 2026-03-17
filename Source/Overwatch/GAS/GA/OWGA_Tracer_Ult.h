// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "OWGA_Tracer_Ult.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWGA_Tracer_Ult : public UOWGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGA_Tracer_Ult();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnThrowBombEvent(FGameplayEventData Payload);
	
	// 던지는 애니메이션
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage>	UltMontage_1P;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage>	UltMontage_3P;

	// 스폰할 투사체 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class AOWProjectile_Tracer_PulseBomb> ProjectileClass;

	// 데미지 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 폭탄이 스폰될 캐릭터 소켓 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SpawnSocketName;

	// 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage = 350.f;

};

