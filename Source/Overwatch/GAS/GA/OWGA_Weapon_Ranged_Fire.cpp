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

	FireShot();
	
	// 타이머 시작 (FireRate 간격으로 반복 호출)
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UOWGA_Weapon_Ranged_Fire::FireShot, FireRate, true);
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
	// 타이머 종료
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOWGA_Weapon_Ranged_Fire::FireShot()
{

	// 탄약 비용 지불
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		// 탄약 부족시 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 무기 체크
	AOWCharacterBase* Character = Cast<AOWCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character) 
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AOWWeapon* Weapon = Character->GetWeapon();
	if (Weapon)
	{
		Weapon->Fire();
	}
}