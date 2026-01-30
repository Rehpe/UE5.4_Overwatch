// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Death.h"

#include "AbilitySystemComponent.h"
#include "Character/OWCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/OWPlayerState.h"


UOWGA_Death::UOWGA_Death()
{
	// 사망 이벤트의 경우 서버에서 승인되어야함
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UOWGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	//Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// GE_Death 적용
	if (DeathEffectClass)
	{
		FGameplayEffectSpecHandle DeathSpec = MakeOutgoingGameplayEffectSpec(DeathEffectClass);
		if (DeathSpec.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, DeathSpec);
		}
	}

	// Ragdoll 처리
	StartRagdoll(TriggerEventData);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UOWGA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOWGA_Death::StartRagdoll(const FGameplayEventData* TriggerEventData)
{
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (!OWCharacter) return;

	// 관성 제거 (이동 정지)
	if (UCharacterMovementComponent* CMC = OWCharacter->GetCharacterMovement())
	{
		CMC->StopMovementImmediately();
		CMC->DisableMovement();
	}

	// 콜리전 해제
	if (UCapsuleComponent* Capsule = OWCharacter->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Ragdoll & Impulse
	if (USkeletalMeshComponent* Mesh = OWCharacter->GetMesh())
	{
		// Ragdoll
		Mesh->SetCollisionProfileName(FName("Ragdoll"));
		Mesh->SetSimulatePhysics(true);

		// Impulse
		FVector ImpulseDirection = FVector::ZeroVector;
		
		if (TriggerEventData && TriggerEventData->ContextHandle.IsValid())
		{
			// HitResult가 있는 경우
			if (const FHitResult* Hit = TriggerEventData->ContextHandle.GetHitResult())
			{
				// TraceStart(총구) -> TraceEnd(맞은 곳) 방향
				ImpulseDirection = (Hit->TraceEnd - Hit->TraceStart).GetSafeNormal();
			}
			// HitResult는 없지만 EffectCauser가 있는 경우 (광역뎀)
			else if (AActor* CauserActor = TriggerEventData->ContextHandle.GetEffectCauser())
			{
				FVector CauserLocation = CauserActor->GetActorLocation();
				FVector MyLocation = OWCharacter->GetActorLocation();
				// 폭발 중심 -> 내 위치 방향
				ImpulseDirection = (MyLocation - CauserLocation).GetSafeNormal();
			}
		}
		
		if (!ImpulseDirection.IsZero())
		{
			// Impulse Debug
			DrawDebugDirectionalArrow(
				GetWorld(), 
				OWCharacter->GetActorLocation(), 
				OWCharacter->GetActorLocation() + (ImpulseDirection * 200.0f), 
				50.0f, FColor::Red, false, 3.0f, 0, 5.0f
			);
			
			float ImpulsePower = 5000.0f;

			// 척추가 있다면 척추를 민다
			FName BoneName = FName("bone_0002");
			if (Mesh->GetBoneIndex(BoneName) != INDEX_NONE)
			{
				Mesh->AddImpulse(ImpulseDirection * ImpulsePower, BoneName, true); 
			}
			else
			{
				Mesh->AddImpulse(ImpulseDirection * ImpulsePower, NAME_None, true); 
			}
		}
	}
}
