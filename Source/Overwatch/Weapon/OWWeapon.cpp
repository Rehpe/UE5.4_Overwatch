// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/OWWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Overwatch.h"
#include "GAS/Tags/OWGameplayTags.h"

class UAbilitySystemComponent;
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
	WeaponMesh3P_Main->SetCastShadow(false);
	WeaponMesh3P_Main->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponMesh3P_Sub = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh3P_Sub"));
	WeaponMesh3P_Sub->SetupAttachment(RootComponent);
	WeaponMesh3P_Sub->SetOwnerNoSee(true);
	WeaponMesh3P_Sub->SetCastShadow(false);
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
	//OWLOG_SCREEN(TEXT("AOWWeapon::Fire Called(Base)"));
}

void AOWWeapon::ApplyWeaponDamage(const FHitResult& HitResult) const
{
	AActor* HitActor = HitResult.GetActor();
	
	if (!HitActor || !DamageEffectClass || HitActor == GetOwner()) return;

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

	if (SourceASC && TargetASC)
	{
		// Context 설정 (누가, 무엇으로 때렸는가)
		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this); 
		ContextHandle.AddHitResult(HitResult);

		// Spec 생성
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			FGameplayTag DamageTag = FOWGameplayTags::Get().Data_Damage; 
			FGameplayTag IdentityTag = FOWGameplayTags::Get().Data_Damage_WeaponFire; 
			float FinalDamage = -BaseWeaponFireDamage;
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, FinalDamage);
			SpecHandle.Data.Get()->AddDynamicAssetTag(IdentityTag);
			// 타겟에게 적용
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
                
			OWLOG_SCREEN(TEXT("Applied %f Weapon Damage to %s"), FinalDamage, *HitActor->GetName());
		}
	}
}


