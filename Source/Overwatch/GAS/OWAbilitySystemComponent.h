// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "OWAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	// 태그로 GA 검색 기능
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	// 초기화 관련 코드
	// virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
