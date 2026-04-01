// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Components/GameFrameworkComponent.h"
#include "GAS/Attributes/OWAttributeSet_Base.h"
#include "GAS/Attributes/OWAttributeSet_Skill.h"
#include "GAS/Attributes/OWAttributeSet_Weapon.h"
#include "OWCombatComponent.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FOWSkillUIData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OW|UI")
	FGameplayTag InputTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OW|UI")
	class UTexture2D* SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OW|UI")
	bool bIsChargeSkill = false;
};

// 수치 변경 (old, new, max)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOWCombat_AttributeChanged,
	FGameplayTag, SlotTag,
	float, OldValue,
	float, NewValue,
	float, MaxValue);

// 쿨타임/리젠 상태용 (시간형)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOWCombat_CooldownChanged,
	FGameplayTag, SlotTag,
	float, TimeRemaining,
	float, TotalDuration);

UCLASS()
class OVERWATCH_API UOWCombatComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UOWCombatComponent(const FObjectInitializer& ObjectInitializer);

	void InitializeWithAbilitySystem(class UAbilitySystemComponent* InASC);

	// HeroData에서 위젯에 배치할 스킬을 분리함
	UFUNCTION(BlueprintCallable)
	void GetHeroSkillLayout(const class UOWHeroData* HeroData, TArray<FOWSkillUIData>& OutSkillData, FOWSkillUIData& OutUltData);

	FORCEINLINE float GetAmmo() const			{ if(WeaponAtt) return WeaponAtt->GetAmmo(); else return 0.f; }
	FORCEINLINE float GetMaxAmmo() const		{ if(WeaponAtt) return WeaponAtt->GetMaxAmmo(); else return 0.f; }
	FORCEINLINE float GetUltCharge() const		{ if(BaseAtt)	return BaseAtt->GetUltimateCharge(); else return 0.f; }
	FORCEINLINE float GetMaxUltCharge() const	{ if(BaseAtt)	return BaseAtt->GetMaxUltimateCharge(); else return 0.f; }
	FORCEINLINE float GetStamina() const		{ if(SkillAtt) return SkillAtt->GetStamina(); else return 0.f; }
	FORCEINLINE float GetMaxStamina() const		{ if(SkillAtt) return SkillAtt->GetMaxStamina(); else return 0.f; }
	FORCEINLINE float GetBarrier() const		{ if(SkillAtt) return SkillAtt->GetBarrier(); else return 0.f; }
	FORCEINLINE float GetMaxBarrier() const		{ if(SkillAtt) return SkillAtt->GetMaxBarrier(); else return 0.f; }
	
public:
	UPROPERTY(BlueprintAssignable) FOWCombat_AttributeChanged OnAttributeChanged;
	UPROPERTY(BlueprintAssignable) FOWCombat_CooldownChanged OnCooldownChanged;

protected:
	// Attribute
	void HandleAmmoChanged(const struct FOnAttributeChangeData& Data);
	void HandleUltChanged(const FOnAttributeChangeData& Data);
	void HandleBarrierChanged(const FOnAttributeChangeData& Data);
	void HandleStaminaChanged(const FOnAttributeChangeData& Data);
	
	// Tag (쿨다운, 리젠)
	void HandleCooldownTagChanged(const FGameplayTag CallbackTag, int32 NewCount, FGameplayTag SlotTag);
	void HandleRegenTagChanged(const FGameplayTag CallbackTag, int32 NewCount, FGameplayTag SlotTag, FGameplayAttribute RegenPeriodAttr);
	
	UPROPERTY() TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY() TObjectPtr<const class UOWAttributeSet_Base> BaseAtt;
	UPROPERTY() TObjectPtr<const class UOWAttributeSet_Weapon> WeaponAtt;
	UPROPERTY() TObjectPtr<const class UOWAttributeSet_Skill> SkillAtt;

private:
	UPROPERTY()
	TMap<FGameplayTag, FGameplayAttribute> SlotTagAttributeMap;
};
