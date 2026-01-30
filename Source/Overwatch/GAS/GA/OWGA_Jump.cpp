// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Jump.h"

#include "Overwatch.h"
#include "Character/OWCharacterBase.h"
#include "GameFramework/Character.h"

UOWGA_Jump::UOWGA_Jump()
{
}

bool UOWGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	const AOWCharacterBase* OWCharacter = GetOWCharacter();
	return OWCharacter && OWCharacter->CanJump();
}

void UOWGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (OWCharacter)
		OWCharacter->Jump();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UOWGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
