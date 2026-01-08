// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "OWAttributeSet.generated.h"

/**
 * 
 */

// 매크로: 속성 접근자(Getter/Setter) 자동 생성 도우미
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class OVERWATCH_API UOWAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UOWAttributeSet();

protected:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	// -- Health -- 
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet, MaxHealth);
	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	// -- Ammo --
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Ammo)
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet, Ammo);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxAmmo)
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet, MaxAmmo);

	UFUNCTION()
	virtual void OnRep_Ammo(const FGameplayAttributeData& OldAmmo);

	UFUNCTION()
	virtual void OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo);

	// -- Ult Charge --
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Ultimate", ReplicatedUsing = OnRep_UltimateCharge)
	FGameplayAttributeData UltimateCharge;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet, UltimateCharge);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Ultimate", ReplicatedUsing = OnRep_MaxUltimateCharge)
	FGameplayAttributeData MaxUltimateCharge;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet, MaxUltimateCharge);

	UFUNCTION()
	virtual void OnRep_UltimateCharge(const FGameplayAttributeData& OldUlt);

	UFUNCTION()
	virtual void OnRep_MaxUltimateCharge(const FGameplayAttributeData& OldMaxUlt);
	
	// -- Replication --
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
