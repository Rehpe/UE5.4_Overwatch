// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_PrimaryAttack.h"
#include "Character/OWCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Overwatch.h"

UOWGA_PrimaryAttack::UOWGA_PrimaryAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UOWGA_PrimaryAttack::InputPressed(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	//부모 구현부가 없기 때문에 굳이 Super을 쓰지 않아도 됨
	//Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	OWLOG_WARNING(TEXT("Begin"));
}

void UOWGA_PrimaryAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AOWCharacterBase* OWCharacterBase = CastChecked<AOWCharacterBase>(ActorInfo->AvatarActor.Get());
	//AbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), OWCharacterBase->GetPrimaryAttackMontage());
	//
	//PlayAttackTask->OnComplete.AddDynamic(this, &UOWGA_PrimaryAttack::OnCompleteCallback);
	//PlayAttackTask->OnInterrupted.AddDynamic(this, &UOWGA_PrimaryAttack::OnInterruptedCallback);
	//PlayAttackTask->ReadyForActivation();
}

void UOWGA_PrimaryAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UOWGA_PrimaryAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOWGA_PrimaryAttack::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;	// 완료 여부를 복제할 것인지
	bool bWasCancelled = false;			// 취소된 경우인지
	EndAbility(CurrentSpecHandle, CurrentActorInfo,  CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UOWGA_PrimaryAttack::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;	// 완료 여부를 복제할 것인지
	bool bWasCancelled = true;			// 취소된 경우인지
	EndAbility(CurrentSpecHandle, CurrentActorInfo,  CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
