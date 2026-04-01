// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "OWUserWidget.generated.h"

/**
 * 
 */


UCLASS()
class OVERWATCH_API UOWUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "OW|UI")
	void InitWidgetWithHealthComp(class UOWHealthComponent* InHealthComp);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "OW|UI")
	void InitWidgetWithCombatComp(class UOWCombatComponent* InCombatComp);

	// BP 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI")
	void InitHeroName(const FText& InHeroName);
	
protected:
	UFUNCTION()
	void HandleHealthChanged(UOWHealthComponent* HealthComp, float OldValue, float NewValue, AActor* Instigator);

	UFUNCTION()
	void HandleMaxHealthChanged(UOWHealthComponent* HealthComp, float OldValue, float NewValue, AActor* Instigator);

	UFUNCTION()
	void HandleAttributeChanged(FGameplayTag SlotTag, float OldValue, float NewValue, float MaxValue);
	
	UFUNCTION()
	void HandleCooldownChanged(FGameplayTag SlotTag, float TimeRemaining, float TotalDuration);
	
	// BP Event
	// 수치 변경 (SlotTag: UI.Ult, UI.Slot.Weapon, InputTag.Ability.Shift 등)
    UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI")
    void OnUIAttributeChanged(FGameplayTag SlotTag, float OldValue, float NewValue, float MaxValue);

    // 게이지 변경 (SlotTag: InputTag.Ability.Shift, InputTag.Ability.E 등)
    UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI")
    void OnUICooldownUpdated(FGameplayTag SlotTag, float TimeRemaining, float TotalDuration);

    // 체력 변경 (SlotTag 없음/고정)
    UFUNCTION(BlueprintImplementableEvent, Category = "OW|UI")
    void OnHealthChanged(float OldValue, float NewValue, float MaxHealth);
};
