// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/OWAttributeSet.h"
#include "OWAttributeSet_Skill.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWAttributeSet_Skill : public UOWAttributeSet
{
	GENERATED_BODY()

public:
	UOWAttributeSet_Skill();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/*// -- Stamina (리젠되는 자원) --
	UPROPERTY(BlueprintReadOnly, Category = "Skill", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Skill, Stamina);

	UPROPERTY(BlueprintReadOnly, Category = "Skill", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Skill, MaxStamina);
	
	UPROPERTY(BlueprintReadOnly, Category = "Skill", ReplicatedUsing = OnRep_StaminaRegenPeriod)
	FGameplayAttributeData StaminaRegenPeriod;	// 회복시간
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Skill, StaminaRegenPeriod);

	UPROPERTY(BlueprintReadOnly, Category = "Skill", ReplicatedUsing = OnRep_StaminaRegenAmount)
	FGameplayAttributeData StaminaRegenAmount;	// 스태미너 회복량(count)
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Skill, StaminaRegenAmount);*/

	// -- Barrier (깨지는 자원) --
	UPROPERTY(BlueprintReadOnly, Category = "Skill", ReplicatedUsing = OnRep_Barrier)
	FGameplayAttributeData Barrier;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Skill, Barrier);

	UPROPERTY(BlueprintReadOnly, Category = "Skill", ReplicatedUsing = OnRep_MaxBarrier)
	FGameplayAttributeData MaxBarrier;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Skill, MaxBarrier);
	
	UPROPERTY(BlueprintReadOnly, Category = "Skill", ReplicatedUsing = OnRep_BarrierRegenRate)
	FGameplayAttributeData BarrierRegenRate;	// 배리어 회복량(float)
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Skill, BarrierRegenRate);

	UPROPERTY(BlueprintReadOnly, Category = "Skill", ReplicatedUsing = OnRep_BarrierRegenDelay)
	FGameplayAttributeData BarrierRegenDelay;	// 배리어 회복하기까지 딜레이
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Skill, BarrierRegenDelay);

protected:
	//UFUNCTION() virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	//UFUNCTION() virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	//UFUNCTION() virtual void OnRep_StaminaRegenPeriod(const FGameplayAttributeData& OldStaminaRegenPeriod);
	//UFUNCTION() virtual void OnRep_StaminaRegenAmount(const FGameplayAttributeData& OldStaminaRegenAmount);

	UFUNCTION() virtual void OnRep_Barrier(const FGameplayAttributeData& OldBarrier);
	UFUNCTION() virtual void OnRep_MaxBarrier(const FGameplayAttributeData& OldMaxBarrier);
	UFUNCTION() virtual void OnRep_BarrierRegenRate(const FGameplayAttributeData& OldBarrierRegenRate);
	UFUNCTION() virtual void OnRep_BarrierRegenDelay(const FGameplayAttributeData& OldBarrierRegenDelay);
};
