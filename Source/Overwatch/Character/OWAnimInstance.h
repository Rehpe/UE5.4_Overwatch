// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OWAnimInstance.generated.h"

class UOWAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UOWAnimInstance();
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void InitASC(UOWAbilitySystemComponent* OWASC);
	
protected:
	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed;
	
	UPROPERTY(BlueprintReadOnly)
	float Direction;
	
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;
	
	UPROPERTY(BlueprintReadOnly)
	FRotator AimRotation;
	
	// 움직여야 하는가? (Idle <-> Run 전환용)
	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFiring;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouching;

private:
	// 매번 Cast하지 않기 위해 포인터 캐싱
	UPROPERTY()
	TObjectPtr<class AOWCharacterBase> Character;

	UPROPERTY()
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY()
	TObjectPtr<class UOWAbilitySystemComponent> ASC;
};
