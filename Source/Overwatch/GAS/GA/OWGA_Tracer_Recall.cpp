// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Tracer_Recall.h"

#include "AbilitySystemComponent.h"
#include "Character/OWCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/OWSnapshotComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Tags/OWGameplayTags.h"

UOWGA_Tracer_Recall::UOWGA_Tracer_Recall()
{
}

void UOWGA_Tracer_Recall::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (!OWCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 무적&은신
	if (RecallStateEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, RecallStateEffectClass);
		if (SpecHandle.IsValid())
		{
			// 나중에 꺼야 하므로 핸들 저장
			ActiveStateEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
	
	// [Client Only] 1p 카메라 연출
	// if (OWChar->IsLocallyControlled()) { OWChar->StartRewindEffect(RecallDuration); }

	// 0.9초 뒤에 실행
	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, RecallDuration);
	if (WaitTask)
	{
		WaitTask->OnFinish.AddDynamic(this, &ThisClass::OnRecallFinished);
		WaitTask->ReadyForActivation();
	}
}

void UOWGA_Tracer_Recall::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
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
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOWGA_Tracer_Recall::OnRecallFinished()
{
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (!OWCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	UOWSnapshotComponent* SnapshotComp = OWCharacter->FindComponentByClass<UOWSnapshotComponent>();
	if (SnapshotComp)
	{
		FOWRecallSnapshot Snapshot;
		if (SnapshotComp->GetRecallTargetSnapshot(Snapshot)) // 저장된 스냅샷(3초 전) 가져오기
		{
			// 순간이동(서버/클라 둘 다)
			OWCharacter->TeleportTo(Snapshot.Location, Snapshot.Rotation, false, true);

			// 텔레포트 직후 캐릭터가 미끄러지는 현상 방지
			if (UCharacterMovementComponent* CMC = Cast<UCharacterMovementComponent>(OWCharacter->GetMovementComponent()))
			{
				CMC->Velocity = FVector::ZeroVector;
				CMC->UpdateComponentVelocity();
			}

			// Recovery는 서버에서만 처리
			if (HasAuthority(&CurrentActivationInfo))
			{
				// 탄약 재장전
				if (RefillAmmoEffectClass)
				{
					FGameplayEffectSpecHandle AmmoSpec = MakeOutgoingGameplayEffectSpec(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, RefillAmmoEffectClass);
					if (AmmoSpec.IsValid())
					{
						ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, AmmoSpec);
					}
				}
				
				// 이전 체력이 현재 체력보다 더 높은 경우에 체력 회복
				float CurrentHealth = OWCharacter->GetHealth();
				if (RecallHpRecoveryEffectClass && Snapshot.Health > CurrentHealth)
				{
					float HealAmount = Snapshot.Health - CurrentHealth;

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
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
