// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Tracer_Recall.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/OWCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/OWSnapshotComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Tags/OWGameplayTags.h"

UOWGA_Tracer_Recall::UOWGA_Tracer_Recall()
{
}

void UOWGA_Tracer_Recall::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (!OWCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UCharacterMovementComponent* CMC = Cast<UCharacterMovementComponent>(OWCharacter->GetMovementComponent());
	if (!CMC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 물리 이동 차단 
	CMC->SetMovementMode(MOVE_None);
	
	// Activate 기준 스냅샷 저장
	UOWSnapshotComponent* SnapshotComp = OWCharacter->FindComponentByClass<UOWSnapshotComponent>();
	if (SnapshotComp)
	{
		bHasValidSnapshot = SnapshotComp->GetRecallTargetSnapshot(CachedSnapshot);
	}

	// 무적&은신
	if (RecallStateEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, RecallStateEffectClass);
		if (SpecHandle.IsValid())
		{
			// 나중에 꺼야 하므로 핸들 저장
			ActiveStateEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
	
	// [서버/클라] 애님 몽타주 재생 / 완료 후 실행할 함수 예약
	if(RecallMontage)
	{
		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				TEXT("RecallMontageTask"),
				RecallMontage,
				1.0f,
				NAME_None,
				true
				);
		
		if (Task)
		{
			Task->OnCompleted.AddDynamic(this, &ThisClass::OnRecallFinished);
			Task->OnInterrupted.AddDynamic(this, &ThisClass::OnRecallFinished);
			Task->OnCancelled.AddDynamic(this, &ThisClass::OnRecallFinished);
			Task->ReadyForActivation();
		}
		
		if (IsLocallyControlled())
		{
			USkeletalMeshComponent* Mesh1p = OWCharacter->GetFirstPersonMesh(); 
			if (Mesh1p && Mesh1p->GetAnimInstance())
			{
				Mesh1p->GetAnimInstance()->Montage_Play(RecallMontage);
			}
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UOWGA_Tracer_Recall::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActiveStateEffectHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			ASC->RemoveActiveGameplayEffect(ActiveStateEffectHandle);
		}
		ActiveStateEffectHandle.Invalidate();
	}
	bool bShouldReplicateEnd = HasAuthority(&CurrentActivationInfo);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bShouldReplicateEnd, bWasCancelled);
}

void UOWGA_Tracer_Recall::OnRecallFinished()
{
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (!OWCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (bHasValidSnapshot)
	{
		OWCharacter->TeleportTo(CachedSnapshot.Location, CachedSnapshot.ControlRotation, false, true);

		// 텔레포트 직후 캐릭터 물리 상태 복구
		if (UCharacterMovementComponent* CMC = Cast<UCharacterMovementComponent>(OWCharacter->GetMovementComponent()))
		{
			CMC->Velocity = FVector::ZeroVector;
			CMC->SetMovementMode(MOVE_Walking);
			CMC->UpdateComponentVelocity();
		}
		
		// 서버에서만 처리
		if (HasAuthority(&CurrentActivationInfo))
		{
			// 탄약 재장전
			if (RefillAmmoEffectClass)
			{
				FGameplayEffectSpecHandle AmmoSpec = MakeOutgoingGameplayEffectSpec(
					CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, RefillAmmoEffectClass);
				if (AmmoSpec.IsValid())
				{
					ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
												   AmmoSpec);
				}
			}

			// 이전 체력이 현재 체력보다 더 높은 경우에 체력 회복
			float CurrentHealth = OWCharacter->GetHealth();
			if (RecallHpRecoveryEffectClass && CachedSnapshot.Health > CurrentHealth)
			{
				float HealAmount = CachedSnapshot.Health - CurrentHealth;

				FGameplayEffectSpecHandle HealSpec = MakeOutgoingGameplayEffectSpec(
					CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, RecallHpRecoveryEffectClass);
				if (HealSpec.IsValid())
				{
					// Data.HealAmount 태그에 회복량 주입 후 SetByCaller
					FGameplayTag HealTag = FOWGameplayTags::Get().Data_HealAmount;
					HealSpec.Data->SetSetByCallerMagnitude(HealTag, HealAmount);
					ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
												   HealSpec);
				}
			}
		}
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}