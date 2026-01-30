// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Crouch.h"

#include "Character/OWCharacterBase.h"

UOWGA_Crouch::UOWGA_Crouch()
{
}

void UOWGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if(!OWCharacter)
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	
	OWCharacter->Crouch();
	
	//if (CommitAbility(Handle, ActorInfo, ActivationInfo))
}

void UOWGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		AOWCharacterBase* OWCharacter = GetOWCharacter();
		if(OWCharacter)
			OWCharacter->UnCrouch();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOWGA_Crouch::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	// 어빌리티 종료
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool UOWGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AOWCharacterBase* OWCharacter = GetOWCharacter();
	return OWCharacter && OWCharacter->CanCrouch();
}
