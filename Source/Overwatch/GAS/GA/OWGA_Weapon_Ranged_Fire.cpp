// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Weapon_Ranged_Fire.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/OWCharacterBase.h"
#include "Weapon/OWWeapon.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Chaos/ChaosEngineInterface.h" // UEPhysicalSurface용

#define SURFACE_HEAD SurfaceType1

UOWGA_Weapon_Ranged_Fire::UOWGA_Weapon_Ranged_Fire()
{
}

void UOWGA_Weapon_Ranged_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{

	AOWCharacterBase* OWCharacter = GetOWCharacter();
	AOWWeapon* Weapon = OWCharacter ? OWCharacter->GetWeapon() : nullptr;
	if (!OWCharacter || !Weapon) return;
    
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FireShot();

	float FireRate = Weapon->GetBaseWeaponFireRate();
	
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
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	AOWWeapon* Weapon = OWCharacter ? OWCharacter->GetWeapon() : nullptr;
    
	if (!OWCharacter || !Weapon) 
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	// 탄약 비용 지불
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		// 탄약 부족시 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 타겟팅 연산
	FHitResult HitResult;
	FVector TraceEnd;
	bool bHit = PerformTargeting(HitResult, TraceEnd);

	// 데미지 적용은 서버에서만
	if (bHit && HasAuthority(&CurrentActivationInfo))
	{
		ApplyDamage(HitResult);
	}

	// GCN 재생
	PlayEffects(bHit, HitResult, TraceEnd);
}

bool UOWGA_Weapon_Ranged_Fire::PerformTargeting(FHitResult& OutHitResult, FVector& OutTraceEnd)
{
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AOWWeapon* Weapon = OWCharacter ? OWCharacter->GetWeapon() : nullptr;

	if (!AvatarActor || !Weapon) return false;

	// 1. 발사 원점과 방향 구하기(카메라 기준)
	FVector StartLocation;
	FRotator ViewRotation;

	AController* OwnerController = Cast<APawn>(AvatarActor)->GetController();
	if (OwnerController)
	{
		// 플레이어의 시선(카메라) 위치와 방향을 가져옴
		OwnerController->GetPlayerViewPoint(StartLocation, ViewRotation);
	}
	else
	{
		// 컨트롤러가 없으면(AI 등) 그냥 액터 위치 사용
		StartLocation = AvatarActor->GetActorLocation();
		ViewRotation = AvatarActor->GetActorRotation();
	}

	// 2. 무기에서 충돌 검사
	bool bHit = Weapon->Fire(StartLocation, ViewRotation.Vector(), OutHitResult);
	
	OutTraceEnd = bHit ? OutHitResult.ImpactPoint : StartLocation + (ViewRotation.Vector() * 10000.0f);

	return bHit;
}


void UOWGA_Weapon_Ranged_Fire::ApplyDamage(const FHitResult& HitResult)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AOWWeapon* Weapon = Cast<AOWCharacterBase>(AvatarActor)->GetWeapon();
	AActor* HitActor = HitResult.GetActor();
		
	if (HitActor && HitActor != AvatarActor && DamageEffectClass)
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (SourceASC && TargetASC)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());

			if (SpecHandle.IsValid())
			{
				// Context 설정 (누가 무엇으로 때렸는가)
				FGameplayEffectContextHandle ContextHandle = SpecHandle.Data.Get()->GetContext();
				ContextHandle.AddSourceObject(Weapon); // 누가 때렸는가? 무기!
				ContextHandle.AddHitResult(HitResult); // 어디를 맞췄는가?

				// 태그 및 수치 설정 
				FGameplayTag DamageTag = FOWGameplayTags::Get().Data_Damage; 
				FGameplayTag IdentityTag = FOWGameplayTags::Get().Data_Damage_WeaponFire; 
					
				float FinalDamage = Weapon->GetBaseWeaponFireDamage(); 

				SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, FinalDamage);
				SpecHandle.Data.Get()->AddDynamicAssetTag(IdentityTag);

				if (HitResult.PhysMaterial.IsValid())
				{
					EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
					// 물리 재질이 머리라면
					if (SurfaceType == SURFACE_HEAD)
					{
						// 헤드샷 태그 추가
						SpecHandle.Data.Get()->AddDynamicAssetTag(FOWGameplayTags::Get().Damage_Modifier_Headshot);
					}
				}
				
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				//OWLOG_SCREEN(TEXT("Applied %f Weapon Damage to %s"), FinalDamage, *HitActor->GetName());
			}
		}
	}
}

void UOWGA_Weapon_Ranged_Fire::PlayEffects(bool bHit, const FHitResult& HitResult, const FVector& TraceEnd)
{
		AActor* AvatarActor = GetAvatarActorFromActorInfo();
		AOWWeapon* Weapon = Cast<AOWCharacterBase>(AvatarActor)->GetWeapon();
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();

		if (!Weapon || !SourceASC) return;

		FGameplayCueParameters CueParams;
		CueParams.EffectCauser = Weapon; 
		CueParams.Instigator = AvatarActor;
		// 맞췄으면 맞춘 위치로, 빗나갔으면 허공(끝점)으로 세팅
		CueParams.Location = bHit ? HitResult.ImpactPoint : TraceEnd; 
    
		SourceASC->ExecuteGameplayCue(FOWGameplayTags::Get().GameplayCue_Character_WeaponFire, CueParams);
}
