// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OWProjectileBase.generated.h"

UCLASS()
class OVERWATCH_API AOWProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AOWProjectileBase();

	FORCEINLINE class USphereComponent* GetCollisionSphere() const { return CollisionSphere;}

protected:
	virtual void BeginPlay() override;

	// 투사체 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	// 충돌 판정용 구체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* CollisionSphere;

	// 서버에서 충돌이 발생했을 때 호출됨
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 투사체 수명이 다했을 때 (사거리 제한)
	virtual void LifeSpanExpired() override;
};
