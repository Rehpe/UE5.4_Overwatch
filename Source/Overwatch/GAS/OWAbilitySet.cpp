// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/OWAbilitySet.h"
#include "GameplayAbilitySpec.h"
#include "OWAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

void UOWAbilitySet::GiveToAbilitySystem(class UOWAbilitySystemComponent* OWASC, UObject* SourceObject) const
{
	check(OWASC);
	// 배열에 있는 모든 어빌리티를 순회하며 등록
	for (const FOWAbilitySet_GameplayAbility& AbilityToGrant : GrantedGameplayAbilities)
	{
		if (!IsValid(AbilityToGrant.Ability)) continue;
		
		// 어빌리티 스펙 생성
		FGameplayAbilitySpec AbilitySpec(AbilityToGrant.Ability);
		// 어빌리티의 근원 (대부분 Hero)
		AbilitySpec.SourceObject = SourceObject;
		// 어빌리티에 태그 부착
		if (AbilityToGrant.InputTag.IsValid())
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
		}
		// ASC에 태그 붙은 어빌리티 등록
		OWASC->GiveAbility(AbilitySpec);
	}
}
