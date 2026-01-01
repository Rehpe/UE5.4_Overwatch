// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/OWAbilitySystemComponent.h"

void UOWAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 내가 가진 모든 어빌리티(Spec) 순회
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		// 어빌리티가 해당 '입력 태그'를 가지고 있는지 확인
		// (DynamicAbilityTags : 런타임에 부여된 태그를 저장하는 곳)
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpec.InputPressed = true;
			
			if (AbilitySpec.IsActive())
			{
				AbilitySpecInputPressed(AbilitySpec);
			}
			else
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UOWAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 내가 가진 모든 어빌리티(Spec) 순회
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		// [디버깅용 로그] 현재 어빌리티의 이름과 보유 태그 vs 들어온 태그 비교
		if (AbilitySpec.Ability)
		{
			UE_LOG(LogTemp, Warning, TEXT("[InputRelease Check] Ability: %s | Has Tags: %s | Input Tag: %s"), 
				*AbilitySpec.Ability->GetName(), 
				*AbilitySpec.DynamicAbilityTags.ToString(), 
				*InputTag.ToString());
		}
		
		// 어빌리티가 해당 '입력 태그'를 가지고 있는지 확인
		// (DynamicAbilityTags : 런타임에 부여된 태그를 저장하는 곳)
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			if (AbilitySpec.IsActive())
			{
				AbilitySpecInputReleased(AbilitySpec);
			}
			AbilitySpec.InputPressed = false;
		}
	}
}
