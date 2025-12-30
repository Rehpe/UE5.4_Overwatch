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

	// 데미지 (일단 변수로 둡니다. 나중에 AttributeSet으로 넘길 예정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	float BaseDamage = 10.0f;
};
