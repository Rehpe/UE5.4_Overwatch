// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/OWAttributeSet_Base.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Overwatch.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Net/UnrealNetwork.h"

UOWAttributeSet_Base::UOWAttributeSet_Base()
{
}

void UOWAttributeSet_Base::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	if (Attribute == GetUltimateChargeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxUltimateCharge());
	}
	if (Attribute == GetDamageAttribute())
	{
		// 데미지의 경우 상한선은 없고, 음수 방어만
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void UOWAttributeSet_Base::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// Damage 검사
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 들어온 데미지를 로컬 변수에 기록 후 0으로 초기화
		float LocalDamage = GetDamage();
		OWLOG_SCREEN(TEXT("LocalDamage : %f"), LocalDamage);
		SetDamage(0.0f);
		
		if (LocalDamage > 0.0f)
		{
			// 이 데미지를 입기 전의 체력을 기록
			float OldHealth = GetHealth(); 
            
			// 데미지 적용 및 Clamp
			float NewHealth = FMath::Clamp(OldHealth - LocalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);

			// 실제로 입힌 유효 데미지 기록
			float ActualDamageDealt = OldHealth - NewHealth;

			// 데미지로 인해 사망한 경우 OutOfHealth 방송
			if (GetHealth() <= 0.0f && OldHealth > 0.0f)
			{
				AActor* Instigator = Data.EffectSpec.GetContext().GetOriginalInstigator();
				AActor* Causer = Data.EffectSpec.GetContext().GetEffectCauser();
				OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, LocalDamage);
			}
			
			// ========= 궁극기 충전 이벤트 ==========
			AActor* AttackerActor = Data.EffectSpec.GetContext().GetOriginalInstigator();
			UAbilitySystemComponent* AttackerASC = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();
			
			// 유효 데미지 && 자해 아닐 경우
			if (ActualDamageDealt > 0.0f && AttackerActor && AttackerActor != GetOwningActor())
			{
				FGameplayEventData Payload;
				Payload.EventTag = FOWGameplayTags::Get().GameplayEvent_Damage_Dealt;
				Payload.Instigator = GetOwningActor(); // 발생자: 맞은 액터
				Payload.Target = AttackerActor;
				Payload.EventMagnitude = ActualDamageDealt; // 실제 데미지

				// 공격자 ASC로 이벤트 발송
				AttackerASC->HandleGameplayEvent(Payload.EventTag, &Payload);
			}
		}
	}
	// Heal 관련
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	// UltCharge 관련
	if (Data.EvaluatedData.Attribute == GetUltimateChargeAttribute())
	{
		SetUltimateCharge(FMath::Clamp(GetUltimateCharge(), 0.0f, GetMaxUltimateCharge()));
	}
}

void UOWAttributeSet_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//COND_NONE : 적군 아군 상관 없이 모두에게 보냄
	//REPNOTIFY_Always : GAS의 Client Prediction을 위해, 언제나 OnRep 실행
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Base, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Base, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Base, MoveSpeed, COND_None, REPNOTIFY_Always);
 
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Base, UltimateCharge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Base, MaxUltimateCharge, COND_None, REPNOTIFY_Always);
}

void UOWAttributeSet_Base::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Base, Health, OldHealth);
}

void UOWAttributeSet_Base::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Base, MaxHealth, OldMaxHealth);
}

void UOWAttributeSet_Base::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Base, MoveSpeed, OldMoveSpeed);
}

void UOWAttributeSet_Base::OnRep_UltimateCharge(const FGameplayAttributeData& OldUlt)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Base, UltimateCharge, OldUlt);
}

void UOWAttributeSet_Base::OnRep_MaxUltimateCharge(const FGameplayAttributeData& OldMaxUlt)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Base, MaxUltimateCharge, OldMaxUlt);
}
