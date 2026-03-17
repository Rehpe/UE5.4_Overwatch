// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/OWProjectile_Tracer_PulseBomb.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/OWCharacterBase.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AOWProjectile_Tracer_PulseBomb::AOWProjectile_Tracer_PulseBomb()
{
	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMesh->SetupAttachment(RootComponent);
	BombMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌은 Sphere가 한다

	if (ProjectileMovement)
	{
		ProjectileMovement->ProjectileGravityScale = 2.0f; // 묵직하게 떨어짐
		ProjectileMovement->bShouldBounce = false;         // 튕기지 않음
	}
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComp->SetupAttachment(RootComponent);
	NiagaraComp->bAutoActivate = false;
	
}

void AOWProjectile_Tracer_PulseBomb::SetDamageSpecHandle(const FGameplayEffectSpecHandle& InSpecHandle)
{
	DamageSpecHandle = InSpecHandle;
}

void AOWProjectile_Tracer_PulseBomb::BeginPlay()
{
	Super::BeginPlay();
	
	if (AActor* MyInstigator = GetInstigator())
	{
		// 투사체는 던진 사람 무시
		if (CollisionSphere)
		{
			CollisionSphere->IgnoreActorWhenMoving(MyInstigator, true);
		}
	}

	if (GetNetMode() != NM_DedicatedServer) // 클라이언트에서만 시각 효과 실행
	{
		if (NiagaraComp && NS_Trail)
		{
			NiagaraComp->SetAsset(NS_Trail);
			NiagaraComp->Activate(); 
		}
	}
}

void AOWProjectile_Tracer_PulseBomb::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 서버에서만 처리
	if (!HasAuthority()) return;

	if (GetWorld()->GetTimerManager().IsTimerActive(ExplosionTimerHandle)) return;

	// 부착 이후로는 이동을 멈추고 충돌체를 끔
	ProjectileMovement->StopMovementImmediately();
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 상대방의 맞은 부위(OtherComp)에 부착
	if (OtherComp)
	{
		AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
	}

	// 폭발 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AOWProjectile_Tracer_PulseBomb::Explode, ExplosionDelay, false);

	bIsAttached = true;
	if (GetNetMode() != NM_DedicatedServer)
	{
		OnRep_IsAttached();
	}
}

void AOWProjectile_Tracer_PulseBomb::Explode()
{
	if (!HasAuthority() || bHasExploded) return;
	bHasExploded = true;

	FVector ExplodeLocation = GetActorLocation();
	AActor* InstigatorActor = GetInstigator(); // 이 폭탄을 던진 사람 (트레이서)
	UAbilitySystemComponent* InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor);

	// PlayEffect
	if (InstigatorASC)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = ExplodeLocation;
		CueParams.Instigator = InstigatorActor;
        
		InstigatorASC->ExecuteGameplayCue(FOWGameplayTags::Get().GameplayCue_Tracer_Ult_Explode, CueParams);
	}

	// 충돌 반경 내의 타겟 검색
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 폭탄 자기 자신만 무시 (던진 사람은 무시 안 함 -> 자폭 가능)

	bool bHit = GetWorld()->OverlapMultiByChannel(
				OverlapResults,
				ExplodeLocation,
				FQuat::Identity,
				ECC_Pawn,
				SphereShape,
				QueryParams
				);

	// 데미지 전달
	if (bHit && DamageSpecHandle.IsValid())
	{
		// 중복 방지
		TSet<AActor*> HitActors;
		
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			AOWCharacterBase* TargetCharacter = Cast<AOWCharacterBase>(Overlap.GetActor());

			if (!TargetCharacter || HitActors.Contains(TargetCharacter))
			{
				continue;
			}

			HitActors.Add(TargetCharacter);
				
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter);
			if (TargetASC)
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpecHandle.Data.Get());

				// Impulse
				if (UCharacterMovementComponent* TargetCMC = TargetCharacter->GetCharacterMovement())
				{
					FVector KnockbackDir = (TargetCharacter->GetActorLocation() - ExplodeLocation).GetSafeNormal();
					KnockbackDir.Z = FMath::Max(KnockbackDir.Z, 0.5f); // 위로 살짝 띄우기
             
					// bVelocityChange = true 로 주어 질량 무시하고 강제 넉백
					TargetCMC->AddImpulse(KnockbackDir * 2000.0f, true); 
				}
			}
		}
	}

	// 액터 파괴
	Destroy();
}

void AOWProjectile_Tracer_PulseBomb::OnRep_IsAttached()
{
	// 부착되었을 때 호출되는 함수로, Trail 이펙트를 끄고 Pulse 이펙트를 켠다
	if (bIsAttached && GetNetMode() != NM_DedicatedServer)
	{
		if (NiagaraComp)
		{
			NiagaraComp->Deactivate();
		}

		// 2. Pulse 스폰
		if (NS_Pulse)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				NS_Pulse,
				RootComponent,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true 
			);
		}
	}
}


void AOWProjectile_Tracer_PulseBomb::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOWProjectile_Tracer_PulseBomb, bIsAttached);
}
