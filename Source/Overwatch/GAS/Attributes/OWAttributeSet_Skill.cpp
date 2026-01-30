// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/OWAttributeSet_Skill.h"

UOWAttributeSet_Skill::UOWAttributeSet_Skill()
{
}

void UOWAttributeSet_Skill::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UOWAttributeSet_Skill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UOWAttributeSet_Skill::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
}

void UOWAttributeSet_Skill::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
}

void UOWAttributeSet_Skill::OnRep_Barrier(const FGameplayAttributeData& OldBarrier)
{
}

void UOWAttributeSet_Skill::OnRep_MaxBarrier(const FGameplayAttributeData& OldMaxBarrier)
{
}
