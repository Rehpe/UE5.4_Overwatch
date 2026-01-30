// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/OWAttributeSet_Weapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UOWAttributeSet_Weapon::UOWAttributeSet_Weapon()
{
}

void UOWAttributeSet_Weapon::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmo());
	}
}

void UOWAttributeSet_Weapon::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetAmmoAttribute())
	{
		// 클램핑
		float CurrentAmmo = GetAmmo();
		SetAmmo(FMath::Clamp(CurrentAmmo, 0.0f, GetMaxAmmo()));

		// 탄약 0이면 자동으로 재장전
		if (CurrentAmmo <= 0.0f)
		{
			AActor* TargetActor = GetOwningActor();
			FGameplayTag OutofAmmoEventTag = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.Weapon.OutOfAmmo"));

			FGameplayEventData Payload;
			Payload.EventTag = OutofAmmoEventTag;
			Payload.Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();
			Payload.Target = TargetActor;
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OutofAmmoEventTag, Payload);
		}
	}
}

void UOWAttributeSet_Weapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//COND_NONE : 적군 아군 상관 없이 모두에게 보냄
	//REPNOTIFY_Always : GAS의 Client Prediction을 위해, 언제나 OnRep 실행
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Weapon, Ammo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOWAttributeSet_Weapon, MaxAmmo, COND_None, REPNOTIFY_Always);
}

void UOWAttributeSet_Weapon::OnRep_Ammo(const FGameplayAttributeData& OldAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Weapon, Ammo, OldAmmo);
}

void UOWAttributeSet_Weapon::OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOWAttributeSet_Weapon, MaxAmmo, OldMaxAmmo);
}
