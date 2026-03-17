// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "OWUserWidget.generated.h"

/**
 * 
 */


UCLASS()
class OVERWATCH_API UOWUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "OW|UI")
	virtual void InitWidgetWithASC(UAbilitySystemComponent* InASC);

	// ★ [핵심] 영웅별 블루프린트에서 "이 스킬 태그를 감시해달라"고 C++에 요청하는 함수
	UFUNCTION(BlueprintCallable, Category = "OW|UI|Cooldown")
	void RegisterSkillCooldown(FGameplayTag CooldownTag, int32 MaxStacks = 1);

protected:
	TWeakObjectPtr<UAbilitySystemComponent> TargetASC;

	// 등록된 스킬 태그와 해당 스킬의 최대 스택 수를 저장하는 맵
	TMap<FGameplayTag, int32> RegisteredSkillTags;

	// =========================================================================
	// 블루프린트(UMG) 이벤트
	// =========================================================================
    
	// ASC가 연결되었을 때 BP에 알려주는 이벤트 (이때 BP에서 RegisterSkillCooldown을 호출하면 됨)
	UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI")
	void OnASCAttached();

	// 공통 속성
	UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI|Attribute")
	void OnHealthChanged(float OldValue, float NewValue, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI|Attribute")
	void OnUltChargeChanged(float OldValue, float NewValue, float MaxUltimateCharge);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI|Attribute")
	void OnAmmoChanged(float OldValue, float NewValue, float MaxAmmo);

	UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI|Attribute")
	void OnBarrierChanged(float OldValue, float NewValue, float MaxBarrier);
	
	// ★ [핵심] 모든 스킬의 쿨타임/스택 변화를 이 함수 하나로 쏴줍니다.
	UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI|Cooldown")
	void OnSkillStateChanged(FGameplayTag SkillTag, int32 AvailableStacks, int32 MaxStacks, float TimeRemaining, float Duration);

private:
	// 콜백함수
	void HealthChangedCallback(const FOnAttributeChangeData& Data);
	void UltChargeChangedCallback(const FOnAttributeChangeData& Data);
	void AmmoChangedCallback(const FOnAttributeChangeData& Data);
	void BarrierChangedCallback(const FOnAttributeChangeData& Data);

    
	// 등록된 모든 스킬 태그의 변화를 받아내는 만능 콜백 함수
	void OnCooldownTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
