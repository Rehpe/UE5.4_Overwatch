// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/OWProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AOWProjectileBase::AOWProjectileBase()
{
	// 투사체 이동은 ProjectileMovement에 위임
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
	SetReplicatingMovement(true);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->InitSphereRadius(5.0f);

	CollisionSphere->SetCollisionProfileName(FName("OW_Projectile"));
	CollisionSphere->OnComponentHit.AddDynamic(this, &AOWProjectileBase::OnProjectileHit);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;

	InitialLifeSpan = 10.0f;
}

void AOWProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOWProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 서버에서만 처리
	if (HasAuthority())
	{
		// 부모는 파괴만
		// 데미지 및 기타 로직은 자식에서 override
		Destroy();
	}
}

void AOWProjectileBase::LifeSpanExpired()
{
	Super::LifeSpanExpired();
}

