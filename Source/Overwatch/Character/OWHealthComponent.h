// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "OWHealthComponent.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EOWDeathState : uint8
{
	NotDead = 0,       // 살아있음
	DeathStarted,      // 죽음 시작 (랙돌 시작)
	DeathFinished      // 죽음 끝 (시체 소멸 / 리스폰 대기)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOWHealth_AttributeChanged,
	UOWHealthComponent*, HealthComponent,
	float, OldValue,
	float, NewValue,
	AActor*, Instigator);

UCLASS()
class OVERWATCH_API UOWHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UOWHealthComponent(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable)
	void InitializeWithAbilitySystem(UAbilitySystemComponent* InASC);	// ASC 등록
	
	UFUNCTION(BlueprintCallable)
	void UninitializeFromAbilitySystem();								// ASC 해제

	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const struct FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude);

	float GetHealth() const;
	float GetMaxHealth() const;

	// AttributeSet에서 호출됨 -> 죽음 시작
	void StartDeath(AActor* Instigator, const struct FGameplayEffectContextHandle& Context);
	
	// GA_Death에서 호출됨 -> 죽음 끝
	UFUNCTION(BlueprintCallable)
	void FinishDeath();
	
	FORCEINLINE EOWDeathState GetDeathState() const { return DeathState; }
	
public:
	UPROPERTY(BlueprintAssignable)
	FOWHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOWHealth_AttributeChanged OnMaxHealthChanged;

protected:
	// 내부 속성 변경 핸들러
	virtual void HandleHealthChanged(const struct FOnAttributeChangeData& Data);
	virtual void HandleMaxHealthChanged(const struct FOnAttributeChangeData& Data);

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<const class UOWAttributeSet_Base> HealthAtt;
	
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EOWDeathState DeathState;

	UFUNCTION()
	void OnRep_DeathState(EOWDeathState OldState);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
