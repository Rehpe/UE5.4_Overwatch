// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Character/OWCharacterBase.h"

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
}

void UOWGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UOWGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	// 정책이 'OnSpawn'이라면, 캐릭터 태어나자마자 스킬 자동 발동 (패시브)
	if (ActivationPolicy == EOWAbilityActivationPolicy::OnSpawn)
	{
		if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

bool UOWGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
		return false;

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	// 해당 GA에 설정된 CooldownTag를 가져옴
	const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();

	// 쿨타임 없는 스킬의 경우 부모에서 판단
	if (!CooldownGE)
	{
		return Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
	}

	// 내부 GE 스택 수를 계산
	int32 CurrentStacks = ASC->GetGameplayEffectCount(CooldownGE->GetClass(), nullptr);

	// 사용할 수 있는 최대 Stack만큼 현재 스택이 쌓였다면
	// (최대 스택 전부 사용하고 스택 쌓이기 기다리는 중이라면) 스킬 불가능
	if (CurrentStacks >= MaxCooldownStacks)
	{
		// 디버깅용 -> MaxStack만큼 Tag가 찼다 : 쿨타임이다 
		//if (OptionalRelevantTags)
		//	OptionalRelevantTags->AppendTags(*CooldownTags);

		return false;
	}
	// CooldownTag가 없거나, 여유 스택이 있다면 사용 가능
	return true;
}

AOWCharacterBase* UOWGameplayAbility::GetOWCharacter() const
{
	if(CurrentActorInfo)
	{
		return Cast<AOWCharacterBase>(CurrentActorInfo->AvatarActor.Get());
	}
	return nullptr;
}
