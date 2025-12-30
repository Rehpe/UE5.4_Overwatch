// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGameplayAbility.h"

#include "AbilitySystemComponent.h"

UOWGameplayAbility::UOWGameplayAbility()
{
	//FPS 게임 필수 설정: 클라이언트에서 먼저 실행하고 서버에 통보 (반응성 최적화)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	// 인스턴싱 정책: 스킬 쓸 때마다 객체 생성
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// 기본 정책: 입력 시 발동
	ActivationPolicy = EOWAbilityActivationPolicy::OnInputTriggered;
}

void UOWGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// 정책이 'OnSpawn'이라면, 캐릭터 태어나자마자 스킬 자동 발동 (패시브)
	if (ActivationPolicy == EOWAbilityActivationPolicy::OnSpawn)
	{
		if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}
