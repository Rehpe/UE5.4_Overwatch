// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/OWAttributeSet_Skill.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UOWAttributeSet_Skill::UOWAttributeSet_Skill()
{
}

void UOWAttributeSet_Skill::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//if (Attribute == GetStaminaAttribute())
	//{
	//	NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	//}
	if (Attribute == GetBarrierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxBarrier());
	}
}

void UOWAttributeSet_Skill::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UOWAttributeSet_Skill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//COND_NONE : 적군 아군 상관 없이 모두에게 보냄
	//REPNOTIFY_Always : GAS의 Client Prediction을 위해, 언제나 OnRep 실행
	//DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Skill, Stamina, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Skill, MaxStamina, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Skill, StaminaRegenPeriod, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Skill, StaminaRegenAmount, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Skill, Barrier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Skill, MaxBarrier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Skill, BarrierRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Skill, BarrierRegenDelay, COND_None, REPNOTIFY_Always);
}

void UOWAttributeSet_Skill::OnRep_Barrier(const FGameplayAttributeData& OldBarrier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Skill, Barrier, OldBarrier);
}

void UOWAttributeSet_Skill::OnRep_MaxBarrier(const FGameplayAttributeData& OldMaxBarrier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Skill, MaxBarrier, OldMaxBarrier);
}

void UOWAttributeSet_Skill::OnRep_BarrierRegenRate(const FGameplayAttributeData& OldBarrierRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Skill, BarrierRegenRate, OldBarrierRegenRate);
}

void UOWAttributeSet_Skill::OnRep_BarrierRegenDelay(const FGameplayAttributeData& OldBarrierRegenDelay)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Skill, BarrierRegenDelay, OldBarrierRegenDelay);
}

/*

void UOWAttributeSet_Skill::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Skill, Stamina, OldStamina);
}

void UOWAttributeSet_Skill::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Skill, MaxStamina, OldMaxStamina);
}

void UOWAttributeSet_Skill::OnRep_StaminaRegenPeriod(const FGameplayAttributeData& OldStaminaRegenPeriod)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Skill, StaminaRegenPeriod, OldStaminaRegenPeriod);
}

void UOWAttributeSet_Skill::OnRep_StaminaRegenAmount(const FGameplayAttributeData& OldStaminaRegenAmount)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Skill, StaminaRegenAmount, OldStaminaRegenAmount);
}

*/
