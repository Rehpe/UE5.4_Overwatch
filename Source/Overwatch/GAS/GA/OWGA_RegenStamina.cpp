// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_RegenStamina.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GAS/Attributes/OWAttributeSet_Skill.h"

UOWGA_RegenStamina::UOWGA_RegenStamina()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UOWGA_RegenStamina::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 만약 켜졌는데 이미 스태미나가 꽉 차있다면 즉시 종료
	float CurrentStamina = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UOWAttributeSet_Skill::GetStaminaAttribute());
	float MaxStamina = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UOWAttributeSet_Skill::GetMaxStaminaAttribute());

	if (CurrentStamina >= MaxStamina)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 타이머 시작
	StartRegenTimer();
}

void UOWGA_RegenStamina::StartRegenTimer()
{
	// ASC에서 StaminaRegenPeriod 읽어옴
	float RegenDelay = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UOWAttributeSet_Skill::GetStaminaRegenPeriodAttribute());

	// 딜레이 생성 후 함수 바인딩
	UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, RegenDelay);
	Task->OnFinish.AddDynamic(this, &UOWGA_RegenStamina::OnDelayFinished);
	Task->ReadyForActivation();
}


void UOWGA_RegenStamina::OnDelayFinished()
{
	// 서버에서 스태미너 충전
	if (HasAuthority(&CurrentActivationInfo))
	{
		// GE_Stamina_Regen 적용
		if (RegenEffectClass)
		{
			ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, RegenEffectClass.GetDefaultObject(), 1.0f);
		}
	}

	// 스태미너가 꽉 찼는지 검사하여, 아니라면 다시 타이머 시작
	float CurrentStamina = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UOWAttributeSet_Skill::GetStaminaAttribute());
	float MaxStamina = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UOWAttributeSet_Skill::GetMaxStaminaAttribute());

	if (CurrentStamina < MaxStamina)
	{
		StartRegenTimer();
	}
	else
	{
		// 꽉 찼으면 스스로 어빌리티 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
