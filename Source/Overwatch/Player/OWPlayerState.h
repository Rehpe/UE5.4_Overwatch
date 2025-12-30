// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "OWPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API AOWPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOWPlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 커스텀 ASC 반환 버전
	class UOWAbilitySystemComponent* GetOWAbilitySystemComponent() const { return ASC; }

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UOWAbilitySystemComponent> ASC;
	
};
