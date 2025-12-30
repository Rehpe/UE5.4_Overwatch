// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/OWPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GAS/OWAbilitySystemComponent.h"

AOWPlayerState::AOWPlayerState()
{
	ASC = CreateDefaultSubobject<UOWAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
}

class UAbilitySystemComponent* AOWPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
