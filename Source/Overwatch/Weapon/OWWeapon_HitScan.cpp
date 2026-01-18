// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/OWWeapon_HitScan.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Overwatch.h"

void AOWWeapon_HitScan::Fire()
{
	Super::Fire();
	
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	// 1. 발사 원점과 방향 구하기 (카메라 기준)
	FVector StartLocation;
	FRotator ViewRotation;

	AController* OwnerController = OwnerActor->GetInstigatorController();
	if (OwnerController)
	{
		// 플레이어의 시선(카메라) 위치와 방향을 가져옴
		OwnerController->GetPlayerViewPoint(StartLocation, ViewRotation);
	}
	else
	{
		// 컨트롤러가 없으면(AI 등) 그냥 액터 위치 사용
		StartLocation = GetActorLocation();
		ViewRotation = GetActorRotation();
	}

	// 2. 레이저 끝점 계산
	FVector EndLocation = StartLocation + (ViewRotation.Vector() * MaxRange);

	// 3. 충돌 검사 (LineTrace)
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);       // 무기 자신 무시
	QueryParams.AddIgnoredActor(OwnerActor); // 쏘는 주인 무시

	// 'Visibility' 채널(일반적인 물체 및 캐릭터)과 충돌 검사
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);

	// 4. Debug Line
	FVector BeamEnd = bHit ? HitResult.ImpactPoint : EndLocation;
	DrawDebugLine(GetWorld(), StartLocation, BeamEnd, FColor::Red, false, 0.5f, 0, 1.0f);

	if (bHit)
	{
		// Debug Point
		DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 0.5f);

		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

			OWLOG_SCREEN(TEXT("Hit : %s"), *HitActor->GetName());
			if (SourceASC && TargetASC && DamageEffectClass)
			{
				// 문맥(Context) : 누가, 누구를, 무엇으로 때렸나 정보
				FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
				ContextHandle.AddSourceObject(this); // 무기정보 등록
				ContextHandle.AddHitResult(HitResult); // HitResult 등록
				FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle); // Spec 생성

				if (SpecHandle.IsValid())
				{
					SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
                    
					OWLOG_SCREEN(TEXT("Applied Damage to %s"), *HitActor->GetName());
				}
			}
		}
	}
}

void AOWWeapon_HitScan::StartFire()
{
	Super::StartFire();
	
	// 첫 발 즉시 발사
	Fire();

	// 이후 타이머 설정 (FireRate 간격으로 Fire 함수 반복 호출)
	// Looping = true
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AOWWeapon_HitScan::Fire, FireRate, true);
}

void AOWWeapon_HitScan::StopFire()
{
	Super::StopFire();
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}
