// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "OWWeapon.generated.h"

USTRUCT(BlueprintType)
struct FWeaponAnimData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage_1P = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage_3P = nullptr;
};

UCLASS()
class OVERWATCH_API AOWWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AOWWeapon();

public:
	void Equip(USkeletalMeshComponent* CharMesh1P, USkeletalMeshComponent* CharMesh3P);
	
	UFUNCTION(BlueprintCallable)
	virtual bool Fire(const FVector& StartLocation, const FVector& ViewDirection, FHitResult& OutHitResult);

	UFUNCTION(BlueprintCallable) FORCEINLINE float GetBaseWeaponFireDamage() const { return BaseWeaponFireDamage; }
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetBaseWeaponFireRate() const { return FireRate; }
	
	virtual void OnRep_Owner() override;
	
protected:
	// ======= Weapon Mesh
	// -- 1p --
	UPROPERTY(EditDefaultsOnly, Category = "Components|1P")
	TObjectPtr<USkeletalMesh> WeaponMesh1P_Main_Asset;

	UPROPERTY(EditDefaultsOnly, Category = "Components|1P")
	TObjectPtr<USkeletalMesh> WeaponMesh1P_Sub_Asset;

	// -- 3p --
	UPROPERTY(EditDefaultsOnly, Category = "Components|3P")
	TObjectPtr<USkeletalMesh> WeaponMesh3P_Main_Asset;

	UPROPERTY(EditDefaultsOnly, Category = "Components|3P")
	TObjectPtr<USkeletalMesh> WeaponMesh3P_Sub_Asset;

	UPROPERTY(Transient) USkeletalMeshComponent* WeaponMesh1P_Main;
	UPROPERTY(Transient) USkeletalMeshComponent* WeaponMesh1P_Sub;
	UPROPERTY(Transient) USkeletalMeshComponent* WeaponMesh3P_Main;
	UPROPERTY(Transient) USkeletalMeshComponent* WeaponMesh3P_Sub;

	// 캐릭터 메인 무기 장착 소켓명
	UPROPERTY(EditDefaultsOnly)
	FName MainSocketName;

	// 캐릭터 서브 무기 장착 소켓명	(쌍권총에만 해당)
	UPROPERTY(EditDefaultsOnly)
	FName SubSocketName;

	// 메인 무기 총구 소켓명
	UPROPERTY(EditDefaultsOnly)
	FName MainMuzzleSocketName;
    
	// 서브 무기 총구 소켓명	(쌍권총에만 해당)
	UPROPERTY(EditDefaultsOnly)
	FName SubMuzzleSocketName;

	// ========= Weapon Data

	// 1발당 기본 공격력
	UPROPERTY(EditDefaultsOnly)
	float BaseWeaponFireDamage;
	
	// 연사 속도
	UPROPERTY(EditAnywhere)
	float FireRate;
	
	// 사거리
	UPROPERTY(EditAnywhere)
	float MaxRange;

	// ========= GCN
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> FireSFX;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraSystem> FireVFX;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> FireCamShake;
	
	// --- dual weapon ---
	UPROPERTY(EditDefaultsOnly)
	bool bIsDualWeapon;				// 쌍수 무기인가?

	UPROPERTY(Transient)
	bool bIsNextFireMain1P;			// 다음 격발은 Main 무기인가?

	UPROPERTY(Transient)
	bool bIsNextFireMain3P;			// 다음 격발은 Main 무기인가?

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, FWeaponAnimData> WeaponMontages;

public:
	UFUNCTION(BlueprintCallable)
	void PlayWeaponVFX();
	
	UFUNCTION(BlueprintCallable)
	void PlayWeaponSFX();
	
	UFUNCTION(BlueprintCallable)
	void PlayWeaponCamShake();
	
	UFUNCTION(BlueprintCallable)
	FWeaponAnimData GetWeaponAnimData(FGameplayTag ActionTag) const
	{
		if (const FWeaponAnimData* FoundData = WeaponMontages.Find(ActionTag))
		{
			return *FoundData;
		}
		return FWeaponAnimData();
	}
};
