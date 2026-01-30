// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/OWAttributeSet.h"
#include "OWAttributeSet_Weapon.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWAttributeSet_Weapon : public UOWAttributeSet
{
	GENERATED_BODY()

public:
	UOWAttributeSet_Weapon();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// -- Ammo --
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Ammo)
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Weapon, Ammo);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxAmmo)
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UOWAttributeSet_Weapon, MaxAmmo);

	UFUNCTION()
	virtual void OnRep_Ammo(const FGameplayAttributeData& OldAmmo);

	UFUNCTION()
	virtual void OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo);
};
