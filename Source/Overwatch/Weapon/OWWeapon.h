// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OWWeapon.generated.h"

UCLASS()
class OVERWATCH_API AOWWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AOWWeapon();

public:
	void Equip(USkeletalMeshComponent* CharMesh1P, USkeletalMeshComponent* CharMesh3P);

	virtual void Fire();
	
protected:
	// -- 1p --
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|1P")
	TObjectPtr<UStaticMeshComponent> WeaponMesh1P_Main;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|1P")
	TObjectPtr<UStaticMeshComponent> WeaponMesh1P_Sub;

	// -- 3p --
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|3P")
	TObjectPtr<UStaticMeshComponent> WeaponMesh3P_Main;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|3P")
	TObjectPtr<UStaticMeshComponent> WeaponMesh3P_Sub;

	// 메인 무기 소켓명
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FName MainSocketName;

	// 서브 무기 소켓명
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FName SubSocketName;

};
