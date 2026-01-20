// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/OWAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Tags/OWGameplayTags.h"

UOWAttributeSet::UOWAttributeSet()
{
	InitHealth(150.0f);        
	InitMaxHealth(150.0f);

	InitAmmo(40.0f);           
	InitMaxAmmo(40.0f);

	InitUltimateCharge(0.0f); 
	InitMaxUltimateCharge(100.0f);
}

void UOWAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 0 미만으로 떨어지는 것 방지
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	if (Attribute == GetAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmo());
	}
}

void UOWAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Health 검사
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		if (GetHealth() <= 0.0f)
		{
			// 때린 사람(Attacker) 정보 가져오기
			AActor* AttackerActor = Data.EffectSpec.GetContext().GetOriginalInstigator();
			FString AttackerName = AttackerActor ? AttackerActor->GetName() : TEXT("Unknown");
			
			FGameplayEventData Payload;
			Payload.EventTag = FOWGameplayTags::Get().Event_Character_Death;
			Payload.Instigator = AttackerActor;
			Payload.Target = GetOwningActor();
			Payload.ContextHandle = Data.EffectSpec.GetContext();
			
			UE_LOG(LogTemp, Error, TEXT("[TAG_CHECK] Sending Tag: %s"), *Payload.EventTag.ToString());
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				GetOwningActor(), 
				Payload.EventTag, 
				Payload
				);
			
			UE_LOG(LogTemp, Error, TEXT("YOU DIED! Killed by: %s"), *AttackerName);
		}
	}
	if (Data.EvaluatedData.Attribute == GetAmmoAttribute())
	{
		SetAmmo(FMath::Clamp(GetAmmo(), 0.0f, GetMaxAmmo()));
		//탄약 0일 때 재장전 
	}
}

void UOWAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//COND_NONE : 적군 아군 상관 없이 모두에게 보냄
	//REPNOTIFY_Always : GAS의 Client Prediction을 위해, 언제나 OnRep 실행
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet, Ammo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet, UltimateCharge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet, MaxUltimateCharge, COND_None, REPNOTIFY_Always);
}

void UOWAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet, Health, OldHealth);
}

void UOWAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet, MaxHealth, OldMaxHealth);
}

void UOWAttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet, Ammo, OldAmmo);
}

void UOWAttributeSet::OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet, MaxAmmo, OldMaxAmmo);
}

void UOWAttributeSet::OnRep_UltimateCharge(const FGameplayAttributeData& OldUlt)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet, UltimateCharge, OldUlt);
}

void UOWAttributeSet::OnRep_MaxUltimateCharge(const FGameplayAttributeData& OldMaxUlt)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet, MaxUltimateCharge, OldMaxUlt);
}