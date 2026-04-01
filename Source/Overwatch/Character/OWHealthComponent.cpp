// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OWHealthComponent.h"

#include "AbilitySystemComponent.h"
#include "GAS/OWAbilitySet.h"
#include "GAS/Attributes/OWAttributeSet_Base.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Net/UnrealNetwork.h"


UOWHealthComponent::UOWHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UOWHealthComponent::InitializeWithAbilitySystem(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;

	ASC = InASC;
	HealthAtt = ASC->GetSet<UOWAttributeSet_Base>();

	if (!HealthAtt) return;
	
	ASC->GetGameplayAttributeValueChangeDelegate(HealthAtt->GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(HealthAtt->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);

	HealthAtt->OnOutOfHealth.AddUObject(this, &UOWHealthComponent::HandleOutOfHealth);
}

void UOWHealthComponent::UninitializeFromAbilitySystem()
{
	if (ASC)
	{
		// 바인딩 해제
	}
	HealthAtt = nullptr;
	ASC = nullptr;
}

void UOWHealthComponent::HandleHealthChanged(const struct FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue, nullptr);
}

void UOWHealthComponent::HandleMaxHealthChanged(const struct FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue, nullptr);
}

void UOWHealthComponent::OnRep_DeathState(EOWDeathState OldState)
{
	
}

void UOWHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude)
{
	if (GetOwner()->HasAuthority())
	{
		// 사망 이벤트 처리
		// EffectSpec에서 Context를 추출
		FGameplayEffectContextHandle ContextHandle = DamageEffectSpec ? DamageEffectSpec->GetContext() : FGameplayEffectContextHandle();
		StartDeath(DamageInstigator, ContextHandle);
	}
}

float UOWHealthComponent::GetHealth() const
{
	if(HealthAtt)
	{
		return HealthAtt->GetHealth();
	}
	return 0.0f;
}

float UOWHealthComponent::GetMaxHealth() const
{
	if(HealthAtt)
	{
		return HealthAtt->GetMaxHealth();
	}
	return 0.0f;
}

void UOWHealthComponent::StartDeath(AActor* Instigator, const FGameplayEffectContextHandle& Context)
{
	// 중복 방지
	if (DeathState != EOWDeathState::NotDead) return;

	DeathState = EOWDeathState::DeathStarted;
	
	// GA_Death 발동을 위한 이벤트 발송
	FGameplayEventData Payload;
	Payload.EventTag = FOWGameplayTags::Get().GameplayEvent_Character_Death;
	Payload.Instigator = Instigator;
	Payload.Target = GetOwner();
	Payload.ContextHandle = Context;

	if (ASC)
	{
		ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

void UOWHealthComponent::FinishDeath()
{
	if (DeathState != EOWDeathState::DeathStarted) return;

	DeathState = EOWDeathState::DeathFinished;
	
	GetOwner()->SetLifeSpan(2.f);
}

void UOWHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UOWHealthComponent, DeathState);
}