// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/OWAttributeSet.h"
#include "OWAttributeSet_Base.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWAttributeSet_Base : public UOWAttributeSet
{
	GENERATED_BODY()

public:
	UOWAttributeSet_Base();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// -- Health -- 
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, MaxHealth);
	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	// -- MoveSpeed --
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, MoveSpeed);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	// -- Ult Charge --
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_UltimateCharge)
	FGameplayAttributeData UltimateCharge;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, UltimateCharge);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxUltimateCharge)
	FGameplayAttributeData MaxUltimateCharge;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Base, MaxUltimateCharge);

	UFUNCTION()
	virtual void OnRep_UltimateCharge(const FGameplayAttributeData& OldUlt);

	UFUNCTION()
	virtual void OnRep_MaxUltimateCharge(const FGameplayAttributeData& OldMaxUlt);
};
