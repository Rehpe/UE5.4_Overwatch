// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/OWWeapon.h"
#include "OWWeapon_HitScan.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API AOWWeapon_HitScan : public AOWWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire() override;

protected:
	// 사거리 (기본 50m)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	float MaxRange = 5000.0f;

	// 연사 속도 (발사 간격, 0.1 = 초당 10발)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	float FireRate = 0.1f;

	// 총알(GE) 종류를 블루프린트에서 설정할 수 있게 변수 추가
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;
};
