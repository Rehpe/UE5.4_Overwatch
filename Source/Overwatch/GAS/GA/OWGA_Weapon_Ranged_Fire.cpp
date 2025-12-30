// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Weapon_Ranged_Fire.h"

#include "Character/OWCharacterBase.h"
#include "Weapon/OWWeapon.h"

UOWGA_Weapon_Ranged_Fire::UOWGA_Weapon_Ranged_Fire()
{
}

void UOWGA_Weapon_Ranged_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 1. 커밋 (쿨타임, 코스트 확인 - 지금은 없지만 습관적으로 넣어둠)
	// if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	// {
	//     EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	//     return;
	// }
	
	AOWCharacterBase* Character = Cast<AOWCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AOWWeapon* Weapon = Character->GetWeapon();
	if (Weapon)
	{
		Weapon->Fire();
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
