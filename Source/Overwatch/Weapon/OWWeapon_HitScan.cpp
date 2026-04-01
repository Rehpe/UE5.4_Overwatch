// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/OWWeapon_HitScan.h"


bool AOWWeapon_HitScan::Fire(const FVector& StartLocation, const FVector& ViewDirection, FHitResult& OutHitResult)
{
	// 레이저 끝점 계산
	const FVector EndLocation = StartLocation + (ViewDirection * MaxRange);

	// 충돌 검사 (LineTrace)
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetInstigator()); // 쏘는 주인 무시 (Character)
	QueryParams.AddIgnoredActor(this); // 무기 자신 무시

	// HeadShot 판정을 위해 PM 설정을 켠다
	QueryParams.bReturnPhysicalMaterial = true;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);

	// 디버그 라인
	FVector BeamEnd = bHit ? OutHitResult.ImpactPoint : EndLocation;
	DrawDebugLine(GetWorld(), StartLocation, BeamEnd, FColor::Red, false, 0.5f, 0, 1.0f);
	DrawDebugPoint(GetWorld(), OutHitResult.ImpactPoint, 3.0f, FColor::Red, false, 2.0f);

	return bHit;
}
