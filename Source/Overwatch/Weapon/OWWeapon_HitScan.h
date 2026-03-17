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
	virtual bool Fire(const FVector& StartLocation, const FVector& ViewDirection, FHitResult& OutHitResult) override;
};
