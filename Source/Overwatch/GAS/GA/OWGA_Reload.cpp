// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Reload.h"

#include "Character/OWCharacterBase.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Weapon/OWWeapon.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UOWGA_Reload::UOWGA_Reload()
{
}

bool UOWGA_Reload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UOWGA_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (!OWCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AOWWeapon* Weapon = OWCharacter->GetWeapon();
	if (!Weapon)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	FGameplayTag ReloadTag = FOWGameplayTags::Get().Ability_Action_Reload;
	FWeaponAnimData AnimData = Weapon->GetWeaponAnimData(ReloadTag);

	// 3p 기준 데이터 없는 경우 취소
	if (!AnimData.Montage_3P)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 3p 기준 몽타주 재생
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AnimData.Montage_3P, 
			1.0f,
			NAME_None,
			false,	// GA 강제종료시 애니메이션도 종료할 것인가?
			1.0f
		);
		
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		MontageTask->ReadyForActivation();

	// 1p일 경우 1p 몽타주 재생
	if (IsLocallyControlled() && AnimData.Montage_1P)
	{
		USkeletalMeshComponent* Mesh1p = OWCharacter->GetFirstPersonMesh(); 
		if (Mesh1p && Mesh1p->GetAnimInstance())
		{
			Mesh1p->GetAnimInstance()->Montage_Play(AnimData.Montage_1P);
		}
	}
	
	FGameplayTag RefillTag = FOWGameplayTags::Get().GameplayEvent_Ability_Reload_Refill;

	// Reload Montage에서 Refill Notify가 올 때까지 대기
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RefillTag, nullptr, false, false
	);
    
	EventTask->EventReceived.AddDynamic(this, &ThisClass::OnRefillEvent);
	EventTask->ReadyForActivation();
	
	//EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UOWGA_Reload::OnMontageFinished()
{
	UE_LOG(LogTemp, Error, TEXT("!!! GA_Reload Finished !!! IsServer: %s"), 
			GetOwningActorFromActorInfo()->HasAuthority() ? TEXT("TRUE") : TEXT("FALSE"));
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UOWGA_Reload::OnRefillEvent(FGameplayEventData Payload)
{
	// 서버권위
	if (GetOwningActorFromActorInfo()->HasAuthority())
	{
		AOWCharacterBase* OWCharacter = GetOWCharacter();
		if (OWCharacter)
		{
			if (ReloadEffectClass)
			{
				ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, ReloadEffectClass.GetDefaultObject(), 1.0f);
			}
		}
	}
}
