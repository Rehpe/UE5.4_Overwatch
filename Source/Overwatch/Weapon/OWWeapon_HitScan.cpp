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

		ApplyWeaponDamage(HitResult);
	}
}
