// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/OWAttributeSet.h"
#include "OWAttributeSet_Base.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_FourParams(FOWOutOfHealthEvent,
	AActor* /*Instigator 가해자(Controller/Pawn)*/,
	AActor* /*Causer 타격 도구(탄환, 스킬액터 등)*/,
	const FGameplayEffectSpec* /*EffectSpec* 사망에 기여한 마지막 GE 정보 (데미지 타입, 태그 등)*/,
	float /*Magnitude 마지막 타격의 실제 데미지 수치*/);

UCLASS()
class OVERWATCH_API UOWAttributeSet_Base : public UOWAttributeSet
{
	GENERATED_BODY()

public:
	UOWAttributeSet_Base();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// -- Health -- 
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, MaxHealth);
	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	// 체력이 0이 되었을 때 쏘는 사망 전용 델리게이트
	mutable FOWOutOfHealthEvent OnOutOfHealth;
	
	// Damage -> Replicate 불필요
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, Damage)
	
	// -- MoveSpeed --
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, MoveSpeed);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	// -- Ult Charge --
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UltimateCharge)
	FGameplayAttributeData UltimateCharge;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, UltimateCharge);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxUltimateCharge)
	FGameplayAttributeData MaxUltimateCharge;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, MaxUltimateCharge);

	UFUNCTION()
	virtual void OnRep_UltimateCharge(const FGameplayAttributeData& OldUlt);

	UFUNCTION()
	virtual void OnRep_MaxUltimateCharge(const FGameplayAttributeData& OldMaxUlt);
};
