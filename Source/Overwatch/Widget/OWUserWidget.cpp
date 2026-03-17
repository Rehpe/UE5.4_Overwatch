// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/OWUserWidget.h"

#include "GAS/Attributes/OWAttributeSet.h"
#include "GAS/Attributes/OWAttributeSet_Base.h"
#include "GAS/Attributes/OWAttributeSet_Skill.h"
#include "GAS/Attributes/OWAttributeSet_Weapon.h"
#include "GameplayEffectTypes.h"

void UOWUserWidget::InitWidgetWithASC(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;
	TargetASC = InASC;

	// Attribute 구독
	TargetASC->GetGameplayAttributeValueChangeDelegate(UOWAttributeSet_Base::GetHealthAttribute())
			 .AddUObject(this, &UOWUserWidget::HealthChangedCallback);
	
	TargetASC->GetGameplayAttributeValueChangeDelegate(UOWAttributeSet_Base::GetUltimateChargeAttribute())
			 .AddUObject(this, &UOWUserWidget::UltChargeChangedCallback);
	
	TargetASC->GetGameplayAttributeValueChangeDelegate(UOWAttributeSet_Weapon::GetAmmoAttribute())
			 .AddUObject(this, &UOWUserWidget::AmmoChangedCallback);

	TargetASC->GetGameplayAttributeValueChangeDelegate(UOWAttributeSet_Skill::GetBarrierAttribute())
			.AddUObject(this, &UOWUserWidget::BarrierChangedCallback);


	// 현재 상태 업데이트

	// Health
	float CurrentHealth = TargetASC->GetNumericAttribute(UOWAttributeSet_Base::GetHealthAttribute());
	float MaxHealth = TargetASC->GetNumericAttribute(UOWAttributeSet_Base::GetMaxHealthAttribute());
	OnHealthChanged(CurrentHealth, CurrentHealth, MaxHealth);

	// UltCharge
	float CurrentUltCharge = TargetASC->GetNumericAttribute(UOWAttributeSet_Base::GetUltimateChargeAttribute());
	float MaxUltCharge = TargetASC->GetNumericAttribute(UOWAttributeSet_Base::GetMaxUltimateChargeAttribute());
	OnUltChargeChanged(CurrentUltCharge, CurrentUltCharge, MaxUltCharge);

	// Ammo
	float CurrentAmmo = TargetASC->GetNumericAttribute(UOWAttributeSet_Weapon::GetAmmoAttribute());
	float MaxAmmo = TargetASC->GetNumericAttribute(UOWAttributeSet_Weapon::GetMaxAmmoAttribute());
	OnAmmoChanged(CurrentAmmo, CurrentAmmo, MaxAmmo);

	// Barrier
	float CurrentBarrier = TargetASC->GetNumericAttribute(UOWAttributeSet_Skill::GetBarrierAttribute());
	float MaxBarrier = TargetASC->GetNumericAttribute(UOWAttributeSet_Skill::GetMaxBarrierAttribute());
	OnBarrierChanged(CurrentBarrier, CurrentBarrier, MaxBarrier);

	// 3. 블루프린트에 "이제 ASC가 연결되었으니 스킬 태그를 등록해라" 라고 알림
	OnASCAttached();
}

void UOWUserWidget::RegisterSkillCooldown(FGameplayTag CooldownTag, int32 MaxStacks)
{
	if (!TargetASC.IsValid() || !CooldownTag.IsValid()) return;

	// 맵에 스킬 태그와 최대 스택 수 저장
	RegisteredSkillTags.Add(CooldownTag, MaxStacks);

	// 해당 태그의 갯수 변화를 구독
	TargetASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::AnyCountChange)
			 .AddUObject(this, &UOWUserWidget::OnCooldownTagChanged);

	// 초기 상태 한 번 밀어넣기
	OnCooldownTagChanged(CooldownTag, TargetASC->GetTagCount(CooldownTag));
}

void UOWUserWidget::HealthChangedCallback(const FOnAttributeChangeData& Data)
{
	if (!TargetASC.IsValid()) return;
	float MaxHealth = TargetASC->GetNumericAttribute(UOWAttributeSet_Base::GetMaxHealthAttribute());
	OnHealthChanged(Data.OldValue, Data.NewValue, MaxHealth);
}

void UOWUserWidget::UltChargeChangedCallback(const FOnAttributeChangeData& Data)
{
	if (!TargetASC.IsValid()) return;
	float MaxUlt = TargetASC->GetNumericAttribute(UOWAttributeSet_Base::GetMaxUltimateChargeAttribute());
	OnUltChargeChanged(Data.OldValue, Data.NewValue, MaxUlt);
}

void UOWUserWidget::AmmoChangedCallback(const FOnAttributeChangeData& Data)
{
	if (!TargetASC.IsValid()) return;
	float MaxAmmo = TargetASC->GetNumericAttribute(UOWAttributeSet_Weapon::GetMaxAmmoAttribute());
	OnAmmoChanged(Data.OldValue, Data.NewValue, MaxAmmo);
}

void UOWUserWidget::BarrierChangedCallback(const FOnAttributeChangeData& Data)
{
	if (!TargetASC.IsValid()) return;
	float MaxBarrier = TargetASC->GetNumericAttribute(UOWAttributeSet_Skill::GetMaxBarrierAttribute());
	OnBarrierChanged(Data.OldValue, Data.NewValue, MaxBarrier);
}

void UOWUserWidget::OnCooldownTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (!TargetASC.IsValid() || !RegisteredSkillTags.Contains(CallbackTag)) return;

	int32 MaxStacks = RegisteredSkillTags[CallbackTag];
	int32 AvailableStacks = FMath::Max(0, MaxStacks - NewCount);

	float TimeRemaining = 0.0f;
	float TotalDuration = 0.0f;

	if (NewCount > 0)
	{
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CallbackTag));
		TArray<float> Durations = TargetASC->GetActiveEffectsTimeRemaining(Query);
		TArray<float> StartTimes = TargetASC->GetActiveEffectsDuration(Query);

		if (Durations.Num() > 0 && StartTimes.Num() > 0)
		{
			TimeRemaining = Durations[0]; 
			TotalDuration = StartTimes[0];
		}
	}

	// 블루프린트로 "이 태그(SkillTag)의 상태가 이렇게 변했다!" 라고 쏴줌
	OnSkillStateChanged(CallbackTag, AvailableStacks, MaxStacks, TimeRemaining, TotalDuration);
}
