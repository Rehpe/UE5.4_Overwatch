// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/OWPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GAS/OWAbilitySystemComponent.h"
#include "GAS/OWAttributeSet.h"

AOWPlayerState::AOWPlayerState()
{
	ASC = CreateDefaultSubobject<UOWAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	
	AttributeSet = CreateDefaultSubobject<UOWAttributeSet>(TEXT("AttributeSet"));
}

class UAbilitySystemComponent* AOWPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
