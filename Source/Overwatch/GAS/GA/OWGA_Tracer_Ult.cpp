// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Tracer_Ult.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/OWCharacterBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Weapon/OWProjectile_Tracer_PulseBomb.h"

UOWGA_Tracer_Ult::UOWGA_Tracer_Ult()
{
}

void UOWGA_Tracer_Ult::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	if(!OWCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 몽타주 재생
	if (UltMontage_3P)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UltMontage_3P);
		MontageTask->OnCompleted.AddDynamic(this, &UOWGA_Tracer_Ult::OnMontageFinished);
		MontageTask->OnInterrupted.AddDynamic(this, &UOWGA_Tracer_Ult::OnMontageFinished);
		MontageTask->OnCancelled.AddDynamic(this, &UOWGA_Tracer_Ult::OnMontageFinished);
		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsLocallyControlled() && UltMontage_1P)
	{
		USkeletalMeshComponent* Mesh1P = OWCharacter->GetFirstPersonMesh();
		if (Mesh1P && Mesh1P->GetAnimInstance())
		{
			Mesh1P->GetAnimInstance()->Montage_Play(UltMontage_1P);
		}
	}

	FGameplayTag PulseBombThrowTag = FOWGameplayTags::Get().GameplayEvent_Ability_Tracer_Ult_ThrowBomb;

	// UltMontage에서 ThrowBomb Notify가 올 때까지 대기
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		PulseBombThrowTag,
		nullptr,
		true,
		false);
	WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnThrowBombEvent);
	WaitEventTask->ReadyForActivation();

	// PlayEffects
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		FGameplayCueParameters CueParams;
		AActor* AvatarActor = GetAvatarActorFromActorInfo(); 
		CueParams.Instigator = AvatarActor;
		if (AvatarActor)
		{
			CueParams.Location = AvatarActor->GetActorLocation();
		}
		FGameplayTag UltCueTag = FOWGameplayTags::Get().GameplayCue_Tracer_Ult_Cast;
		
		ASC->ExecuteGameplayCue(UltCueTag, CueParams);
	}

}

void UOWGA_Tracer_Ult::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UOWGA_Tracer_Ult::OnThrowBombEvent(FGameplayEventData Payload)
{
	// 서버에서만 처리
	if (!HasAuthority(&CurrentActivationInfo)) return;
	
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter || !ProjectileClass || !DamageEffectClass) return;

	FTransform SpawnTransform;

	// 플레이어 시야에서 던짐
	FVector SpawnLocation;
	FRotator SpawnRotation;
	AvatarCharacter->GetActorEyesViewPoint(SpawnLocation, SpawnRotation);
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());

	// Damage Spec Handle 작성
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AOWCharacterBase* OWCharacter = Cast<AOWCharacterBase>(AvatarCharacter);

	// ContextHandle
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddInstigator(OWCharacter, OWCharacter);
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle DamageSpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);
	
	if (DamageSpecHandle.IsValid())
	{
		FGameplayTag DamageTag = FOWGameplayTags::Get().Data_Damage; 
		FGameplayTag IdentityTag = FOWGameplayTags::Get().Data_Damage_Tracer_Ult;

		float FinalDamage = BaseDamage;
		
		DamageSpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, FinalDamage);
		DamageSpecHandle.Data.Get()->AddDynamicAssetTag(IdentityTag);
	}
	
	// 폭탄 스폰 준비
	AOWProjectile_Tracer_PulseBomb* PulseBomb = GetWorld()->SpawnActorDeferred<AOWProjectile_Tracer_PulseBomb>(
		ProjectileClass, 
		SpawnTransform, 
		AvatarCharacter, // Owner
		AvatarCharacter, // Instigator
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (PulseBomb)
	{
		// 던지는 캐릭터의 콜리전 무시
		if (USphereComponent* Sphere = PulseBomb->GetCollisionSphere())
		{
			Sphere->IgnoreActorWhenMoving(AvatarCharacter, true);
		}
		// 폭탄에 DamageSpecHandle 부착 및 최종 스폰
		PulseBomb->SetDamageSpecHandle(DamageSpecHandle);
		PulseBomb->FinishSpawning(SpawnTransform);
	}
}
	