// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Weapon/OWProjectileBase.h"
#include "OWProjectile_Tracer_PulseBomb.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API AOWProjectile_Tracer_PulseBomb : public AOWProjectileBase
{
	GENERATED_BODY()

public:
	AOWProjectile_Tracer_PulseBomb();

	// GA 데미지 정보 세팅용
	void SetDamageSpecHandle(const FGameplayEffectSpecHandle& InSpecHandle);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;

	// 부착 후
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
	void Explode();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OW|Components")
	TObjectPtr<UStaticMeshComponent> BombMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OW|Components")
	TObjectPtr<class UNiagaraComponent> NiagaraComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Components")
	TObjectPtr<class UNiagaraSystem> NS_Trail;	// 투척 시 이펙트
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Components")
	TObjectPtr<class UNiagaraSystem> NS_Pulse;	// 부착 시 이펙트

	// 폭발 지연 시간 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|PulseBomb")
	float ExplosionDelay = 1.5f;

	// 데미지 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|PulseBomb")
	float ExplosionRadius = 500.0f;

	UPROPERTY(ReplicatedUsing=OnRep_IsAttached)
	bool bIsAttached = false;

	UFUNCTION()
	void OnRep_IsAttached();

private:
	FGameplayEffectSpecHandle DamageSpecHandle;

	FTimerHandle ExplosionTimerHandle;
	bool bHasExploded = false;
	
};
