// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_MeleeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Overwatch.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/OWCharacterBase.h"
#include "Character/OWHeroData.h"
#include "GAS/Attributes/OWAttributeSet.h"
#include "GAS/Attributes/OWAttributeSet_Base.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Weapon/OWWeapon.h"

class UAbilityTask_WaitGameplayEvent;

UOWGA_MeleeAttack::UOWGA_MeleeAttack()
{
}

void UOWGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	HitActors.Empty();
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (!OWCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AOWWeapon* Weapon = OWCharacter->GetWeapon();
	FGameplayTag MeleeAttackTag = FOWGameplayTags::Get().Ability_Action_MeleeAttack;
	FWeaponAnimData AnimData = Weapon->GetWeaponAnimData(MeleeAttackTag);
	
	// 3p 기준 데이터 없는 경우 취소
	if (!AnimData.Montage_3P)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	else
	{
		// 3p 기준 몽타주 재생
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				TEXT("MeleeAttackMontage"),
				AnimData.Montage_3P, 
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
		FGameplayTag MeleeHitTag = FOWGameplayTags::Get().GameplayEvent_Melee_Hit;

		// MeleeAttack Montage에서 MeleeHit Notify가 올 때까지 대기
		UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			MeleeHitTag,
			nullptr,
			false,
			false
		);
    
		EventTask->EventReceived.AddDynamic(this, &ThisClass::OnMeleeHitEvent);
		EventTask->ReadyForActivation();

		PlayEffects();
	}
}

void UOWGA_MeleeAttack::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UOWGA_MeleeAttack::OnMeleeHitEvent(FGameplayEventData Payload)
{
	// 조작감을 위해 클라이언트 예측 + 서버 권위
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;
	
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if(!OWCharacter) return;
	
	// Trace
	FHitResult HitResult;
	bool bHit = CheckMeleeHitTrace(HitResult);
	
	if (bHit)
	{
		AActor* TargetActor = HitResult.GetActor();
		if (TargetActor)
		{
			// 이미 명단에 있다면 중복 -> 무시
			if (HitActors.Contains(TargetActor))
				return;

			// 명단에 없다면 추가한다
			HitActors.Add(TargetActor);
			
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

			if(!SourceASC || !TargetASC) return;
			
			// 죽음 / 무적 / 지정불가시 무시
			if (TargetASC->HasMatchingGameplayTag(FOWGameplayTags::Get().State_Dead)||
				TargetASC->HasMatchingGameplayTag(FOWGameplayTags::Get().State_Invulnerable) ||
				TargetASC->HasMatchingGameplayTag(FOWGameplayTags::Get().State_Untargetable))
			{
				return;
			}

			// 아군일 경우 무시

			// 피격용 GCN
			
			// 데미지는 서버만
			if(OWCharacter->HasAuthority())
			{
				if (SourceASC && TargetASC && DamageEffectClass)
				{
					// 문맥(Context) : 누가, 누구를, 무엇으로 때렸나 정보
					FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this); // 무기정보 등록
					ContextHandle.AddHitResult(HitResult); // HitResult 등록
				
					FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle); // Spec 생성
					UOWHeroData* HeroData = OWCharacter->GetHeroData();
					if(!HeroData) return;
				
					if (SpecHandle.IsValid() && HeroData->BaseMeleeAttackDamage > 0.0f)
					{
						FGameplayTag DamageTag = FOWGameplayTags::Get().Data_Damage; 
						FGameplayTag IdentityTag = FOWGameplayTags::Get().Data_Damage_Melee;
						float FinalDamage = HeroData->BaseMeleeAttackDamage;
						SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, FinalDamage);
						SpecHandle.Data.Get()->AddDynamicAssetTag(IdentityTag);
						SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					
						//OWLOG_SCREEN(TEXT("Applied %f Weapon Damage to %s"), FinalDamage, *TargetActor->GetName());
					}
				}
			}
		}
	}
}

void UOWGA_MeleeAttack::PlayEffects()
{
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
		FGameplayTag MeleeAttackCueTag = FOWGameplayTags::Get().GameplayCue_Character_MeleeAttack;
		
		ASC->ExecuteGameplayCue(MeleeAttackCueTag, CueParams);
	}
}

bool UOWGA_MeleeAttack::CheckMeleeHitTrace(FHitResult& OutHitResult)
{
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!AvatarCharacter) return false;

	APlayerController* PC = Cast<APlayerController>(AvatarCharacter->GetController());
	if (!PC) return false;

	// 카메라 위치와 회전값 가져오기 (크로스헤어 기준)
	FVector TraceStart;
	FRotator TraceRot;
	PC->GetPlayerViewPoint(TraceStart, TraceRot);

	// 트레이스 설정: 반경 35cm의 구체를 카메라 전방 150cm까지 쏨
	float SphereRadius = 35.0f;
	float MeleeRange = 150.0f;
	FVector TraceEnd = TraceStart + (TraceRot.Vector() * MeleeRange);

	// 자기 자신은 맞지 않도록 예외 처리
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarCharacter);
	QueryParams.bReturnPhysicalMaterial = true; // 머티리얼별 타격음/이펙트를 위해 true
	QueryParams.bTraceComplex = false; // 단순 콜리전(캡슐 등)을 우선 검사

	UWorld* World = AvatarCharacter->GetWorld();
    
	// 타격 판정 실행 (보통 Pawn이나 캐릭터 전용 트레이스 채널 사용)
	bool bHit = World->SweepSingleByChannel(
		OutHitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn, // 프로젝트 세팅에 맞춰 커스텀 채널(예: ECC_GameTraceChannel1)로 변경 권장
		FCollisionShape::MakeSphere(SphereRadius),
		QueryParams
	);

//#if ENABLE_DRAW_DEBUG
//	// 디버그 드로잉: 맞았으면 녹색, 빗나갔으면 빨간색
//	FColor DrawColor = bHit ? FColor::Green : FColor::Red;
//	DrawDebugCapsule(World, TraceStart + (TraceEnd - TraceStart) * 0.5f, MeleeRange * 0.5f, SphereRadius, TraceRot.Quaternion(), DrawColor, false, 2.0f);
//#endif

	return bHit;
}
