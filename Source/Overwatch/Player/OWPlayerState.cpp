// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/OWPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GAS/OWAbilitySystemComponent.h"
#include "GAS/Attributes/OWAttributeSet_Base.h"
#include "GAS/Attributes/OWAttributeSet_Skill.h"
#include "GAS/Attributes/OWAttributeSet_Weapon.h"

AOWPlayerState::AOWPlayerState()
{
	ASC = CreateDefaultSubobject<UOWAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet_Base = CreateDefaultSubobject<UOWAttributeSet_Base>(TEXT("AttributeSet_Base"));
	AttributeSet_Weapon = CreateDefaultSubobject<UOWAttributeSet_Weapon>(TEXT("AttributeSet_Weapon"));
	AttributeSet_Skill = CreateDefaultSubobject<UOWAttributeSet_Skill>(TEXT("AttributeSet_Skill"));
}

class UAbilitySystemComponent* AOWPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
