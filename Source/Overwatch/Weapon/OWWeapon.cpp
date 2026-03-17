// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/OWWeapon.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/OWCharacterBase.h"
#include "Kismet/GameplayStatics.h"

class UAbilitySystemComponent;
// Sets default values
AOWWeapon::AOWWeapon()
	: bIsDualWeapon(false)
	, bIsNextFireMain1P(true)
	, bIsNextFireMain3P(true)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	bNetUseOwnerRelevancy = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// -- 1p --
	WeaponMesh1P_Main = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P_Main"));
	WeaponMesh1P_Main->SetupAttachment(RootComponent);
	WeaponMesh1P_Main->SetOnlyOwnerSee(true); 
	WeaponMesh1P_Main->SetCastShadow(false);
	WeaponMesh1P_Main->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponMesh1P_Sub = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P_Sub"));
	WeaponMesh1P_Sub->SetupAttachment(RootComponent);
	WeaponMesh1P_Sub->SetOnlyOwnerSee(true);
	WeaponMesh1P_Sub->SetCastShadow(false);
	WeaponMesh1P_Sub->SetCollisionProfileName(TEXT("NoCollision"));
	
	// -- 3p --
	WeaponMesh3P_Main = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P_Main"));
	WeaponMesh3P_Main->SetupAttachment(RootComponent);
	WeaponMesh3P_Main->SetOwnerNoSee(true);  
	WeaponMesh3P_Main->SetCastShadow(false);
	WeaponMesh3P_Main->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponMesh3P_Sub = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P_Sub"));
	WeaponMesh3P_Sub->SetupAttachment(RootComponent);
	WeaponMesh3P_Sub->SetOwnerNoSee(true);
	WeaponMesh3P_Sub->SetCastShadow(false);
	WeaponMesh3P_Sub->SetCollisionProfileName(TEXT("NoCollision"));
}

void AOWWeapon::Equip(USkeletalMeshComponent* CharMesh1P, USkeletalMeshComponent* CharMesh3P)
{
	if (!CharMesh1P || !CharMesh3P) return;

	WeaponMesh1P_Main->SetSkeletalMesh(WeaponMesh1P_Main_Asset);
	WeaponMesh1P_Sub->SetSkeletalMesh(WeaponMesh1P_Sub_Asset);
	WeaponMesh3P_Main->SetSkeletalMesh(WeaponMesh3P_Main_Asset);
	WeaponMesh3P_Sub->SetSkeletalMesh(WeaponMesh3P_Sub_Asset);

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

	// SubMesh가 차있다면 DualWeapon
	if(WeaponMesh1P_Sub != nullptr && WeaponMesh3P_Sub != nullptr)
		bIsDualWeapon = true;
}

// 순수가상함수
bool AOWWeapon::Fire(const FVector& StartLocation, const FVector& ViewDirection, FHitResult& OutHitResult)
{
	return false;
}

void AOWWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (AOWCharacterBase* OWCharacter = Cast<AOWCharacterBase>(GetOwner()))
	{
		OWCharacter->OnWeaponSet(this);
	}
}

void AOWWeapon::PlayWeaponVFX()
{
	if (!FireVFX) return;
	
	bool bIs1P = false;
	APawn* InstigatorPawn = Cast<APawn>(GetInstigator());
	if (InstigatorPawn && InstigatorPawn->IsLocallyControlled())
	{
		bIs1P = true;
	}
	
	USkeletalMeshComponent* ResultMesh = nullptr;
	FName ResultSocket = MainMuzzleSocketName; // 기본 소켓

	// 양손 무기
	if (bIsDualWeapon)
	{
		if (bIs1P)
		{
			ResultMesh = bIsNextFireMain1P ? WeaponMesh1P_Main : WeaponMesh1P_Sub;
			ResultSocket = bIsNextFireMain1P ? MainMuzzleSocketName : SubMuzzleSocketName;
			bIsNextFireMain1P = !bIsNextFireMain1P; 
		}
		else
		{
			ResultMesh = bIsNextFireMain3P ? WeaponMesh3P_Main : WeaponMesh3P_Sub;
			ResultSocket = bIsNextFireMain3P ? MainMuzzleSocketName : SubMuzzleSocketName;
			bIsNextFireMain3P = !bIsNextFireMain3P; 
		}
	}
	// 싱글 무기
	else
	{
		ResultMesh = bIs1P ? WeaponMesh1P_Main : WeaponMesh3P_Main;
		ResultSocket = MainMuzzleSocketName;
	}

	// 메쉬/소켓에 VFX 스폰
	if (ResultMesh)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			FireVFX,
			ResultMesh,
			ResultSocket, 
			FVector::ZeroVector,
			FRotator::ZeroRotator, 
			EAttachLocation::SnapToTarget,
			true
		);
	}
}

void AOWWeapon::PlayWeaponSFX()
{
	if (FireSFX)
	{
		// 무기 위치에서 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(this, FireSFX, GetActorLocation());
	}
}

void AOWWeapon::PlayWeaponCamShake()
{
	if (FireCamShake)
	{
		// 무기를 들고 있는 주인을 찾아서 화면 흔들기
		APawn* InstigatorPawn = Cast<APawn>(GetInstigator());
		if (InstigatorPawn)
		{
			APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
			if (PC && PC->IsLocalController()) // 본인 화면만 흔들리게 처리
			{
				PC->ClientStartCameraShake(FireCamShake);
			}
		}
	}
}



