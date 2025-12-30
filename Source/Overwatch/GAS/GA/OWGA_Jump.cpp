// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Jump.h"

#include "Overwatch.h"
#include "GameFramework/Character.h"

UOWGA_Jump::UOWGA_Jump()
{
}

bool UOWGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	// 언리얼 기본 CanJump 함수로 점프 가능한지 파악
	return Character && Character->CanJump();
}

void UOWGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	OWLOG_SCREEN(TEXT("JumpGA_Activated"));
	
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->Jump();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UOWGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	/*ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		Character->StopJumping();
	}*/
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
