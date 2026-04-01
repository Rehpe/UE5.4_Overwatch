// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/OWUserWidget.h"

#include "Character/OWCombatComponent.h"
#include "Character/OWHealthComponent.h"
#include "GAS/Tags/OWGameplayTags.h"


void UOWUserWidget::InitWidgetWithHealthComp_Implementation(class UOWHealthComponent* InHealthComp)
{
	if (!InHealthComp) return;

	// HealthComponent 델리게이트 구독 (ASC 대신 HealthComp를 구독함)
	InHealthComp->OnHealthChanged.AddDynamic(this, &UOWUserWidget::HandleHealthChanged);
	InHealthComp->OnMaxHealthChanged.AddDynamic(this, &UOWUserWidget::HandleMaxHealthChanged);

	// 체력 초기화
	float CurrentHealth = InHealthComp->GetHealth();
	float MaxHealth = InHealthComp->GetMaxHealth();
	OnHealthChanged(CurrentHealth, CurrentHealth, MaxHealth);
}

void UOWUserWidget::InitWidgetWithCombatComp_Implementation(class UOWCombatComponent* InCombatComp)
{
	if (!InCombatComp) return;
	
	InCombatComp->OnAttributeChanged.AddDynamic(this, &ThisClass::HandleAttributeChanged);
	InCombatComp->OnCooldownChanged.AddDynamic(this, &ThisClass::HandleCooldownChanged);

	/*// 탄약 초기화
	float CurrentAmmo = InCombatComp->GetAmmo();
	float MaxAmmo = InCombatComp->GetMaxAmmo();
	FGameplayTag AmmoTag = FOWGameplayTags::Get().UI_Ammo;
	OnUIAttributeChanged(AmmoTag, CurrentAmmo, CurrentAmmo, MaxAmmo);

	// 궁극기 충전 초기화
	float CurrentUltCharge = InCombatComp->GetUltCharge();
	float MaxUltCharge = InCombatComp->GetMaxUltCharge();
	FGameplayTag UltChargeTag = FOWGameplayTags::Get().UI_Ult;
	OnUIAttributeChanged(CurrentHealth, CurrentHealth, MaxHealth);*/
}

void UOWUserWidget::HandleHealthChanged(UOWHealthComponent* HealthComp, float OldValue, float NewValue,
	AActor* Instigator)
{
	OnHealthChanged(OldValue, NewValue, HealthComp->GetMaxHealth());
}

void UOWUserWidget::HandleMaxHealthChanged(UOWHealthComponent* HealthComp, float OldValue, float NewValue,
	AActor* Instigator)
{
	// 최대 체력이 변하면 비율 갱신을 위해 현재 체력을 다시 보냄
	float CurrentHealth = HealthComp->GetHealth();
	OnHealthChanged(CurrentHealth, CurrentHealth, NewValue);
}

void UOWUserWidget::HandleAttributeChanged(FGameplayTag SlotTag, float OldValue, float NewValue, float MaxValue)
{
	OnUIAttributeChanged(SlotTag, OldValue, NewValue, MaxValue);
}

void UOWUserWidget::HandleCooldownChanged(FGameplayTag SlotTag, float TimeRemaining, float TotalDuration)
{
	OnUICooldownUpdated(SlotTag, TimeRemaining, TotalDuration);
}