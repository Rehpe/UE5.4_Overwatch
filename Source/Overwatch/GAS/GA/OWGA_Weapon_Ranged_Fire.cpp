// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Weapon_Ranged_Fire.h"

#include "Character/OWCharacterBase.h"
#include "Weapon/OWWeapon.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

UOWGA_Weapon_Ranged_Fire::UOWGA_Weapon_Ranged_Fire()
{
}

void UOWGA_Weapon_Ranged_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// 1. 커밋 (쿨타임, 코스트 확인)
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
		Weapon->StartFire();
		/*
		// 키 뗄 때까지 대기하는 Task 생성
		UAbilityTask_WaitInputRelease* WaitTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
		if (WaitTask)
		{
			// 키 떼면 OnRelease 실행
			WaitTask->OnRelease.AddDynamic(this, &UOWGA_Weapon_Ranged_Fire::OnRelease);
			WaitTask->ReadyForActivation();
		}*/
	}
}

void UOWGA_Weapon_Ranged_Fire::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UOWGA_Weapon_Ranged_Fire::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateEndAbility, bool bWasCancelled)
{
	// 이미 종료 중이면 스킵
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		StopRangedWeaponFire();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOWGA_Weapon_Ranged_Fire::StopRangedWeaponFire()
{
	AOWCharacterBase* Character = Cast<AOWCharacterBase>(GetCurrentActorInfo()->AvatarActor.Get());
	if (Character && Character->GetWeapon())
	{
		// 연사 중지(Timer Off)
		Character->GetWeapon()->StopFire();
	}
}