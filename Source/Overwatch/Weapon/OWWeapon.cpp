// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/OWWeapon.h"

#include "Overwatch.h"

// Sets default values
AOWWeapon::AOWWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// -- 1p --
	WeaponMesh1P_Main = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh1P_Main"));
	WeaponMesh1P_Main->SetupAttachment(RootComponent);
	WeaponMesh1P_Main->SetOnlyOwnerSee(true); 
	WeaponMesh1P_Main->SetCastShadow(false);
	WeaponMesh1P_Main->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponMesh1P_Sub = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh1P_Sub"));
	WeaponMesh1P_Sub->SetupAttachment(RootComponent);
	WeaponMesh1P_Sub->SetOnlyOwnerSee(true);
	WeaponMesh1P_Sub->SetCastShadow(false);
	WeaponMesh1P_Sub->SetCollisionProfileName(TEXT("NoCollision"));
	
	// -- 3p --
	WeaponMesh3P_Main = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh3P_Main"));
	WeaponMesh3P_Main->SetupAttachment(RootComponent);
	WeaponMesh3P_Main->SetOwnerNoSee(true);  
	WeaponMesh3P_Main->bCastHiddenShadow = true;
	WeaponMesh3P_Main->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponMesh3P_Sub = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh3P_Sub"));
	WeaponMesh3P_Sub->SetupAttachment(RootComponent);
	WeaponMesh3P_Sub->SetOwnerNoSee(true);
	WeaponMesh3P_Sub->bCastHiddenShadow = true;
	WeaponMesh3P_Sub->SetCollisionProfileName(TEXT("NoCollision"));
}

void AOWWeapon::Equip(USkeletalMeshComponent* CharMesh1P, USkeletalMeshComponent* CharMesh3P)
{
	if (!CharMesh1P || !CharMesh3P) return;

	// 1p 무기는 1p 팔에 부착
	if (WeaponMesh1P_Main)
		WeaponMesh1P_Main->AttachToComponent(CharMesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, MainSocketName);
    
	if (WeaponMesh1P_Sub && !SubSocketName.IsNone())
		WeaponMesh1P_Sub->AttachToComponent(CharMesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, SubSocketName);

	// 3p 무기는 3p 몸에 부착
	if (WeaponMesh3P_Main)
		WeaponMesh3P_Main->AttachToComponent(CharMesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, MainSocketName);

	if (WeaponMesh3P_Sub && !SubSocketName.IsNone())
		WeaponMesh3P_Sub->AttachToComponent(CharMesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, SubSocketName);

	// 액터 Root는 bone_0002(Pelvis) 에 부착
	this->AttachToComponent(CharMesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("bone_0002"));
}

void AOWWeapon::Fire()
{
	OWLOG_SCREEN(TEXT("AOWWeapon::Fire Called(Base)"));
}



