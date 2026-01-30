// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/OWAttributeSet_Base.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
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
}

void UOWAttributeSet_Base::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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
			Payload.EventTag = FOWGameplayTags::Get().GameplayEvent_Character_Death;
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
