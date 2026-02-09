// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/OWGA_Tracer_Blink.h"

#include "Character/OWCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"

UOWGA_Tracer_Blink::UOWGA_Tracer_Blink()
	: BlinkDistance(700.0f)
	, BlinkDuration(0.1f)
	, bEnableGravity(false)
{
}

void UOWGA_Tracer_Blink::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Cost
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AOWCharacterBase* OWCharacter = GetOWCharacter();
	if (!OWCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 이동 방향 계산 (Input Direction)
	// GetLastMovementInputVector(): 컨트롤러 기준 입력값 (WASD)을 월드 벡터로 반환함.
	FVector MoveDir = OWCharacter->GetLastMovementInputVector();
	
	// Input이 없으면 캐릭터가 현재 바라보는 방향(Forward)으로 설정
	if (MoveDir.IsNearlyZero())
	{
		MoveDir = OWCharacter->GetActorForwardVector();
	}
	else
	{
		MoveDir.Normalize();
	}

	// 힘 = 거리 / 시간
	float ForceMagnitude = BlinkDistance / BlinkDuration;

	// RootMotion Task 생성 (Constant Force)
	// 이 Task는 CMC(CharacterMovementComponent)의 RootMotionSource에 접근하여
	// 클라이언트 예측(Predict)과 서버 권한(Server Auth) 이동을 처리합니다.
	FName TaskInstanceName = TEXT("Task_BlinkRootMotion");
	
	UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		TaskInstanceName,
		MoveDir,         // 힘을 가할 방향
		ForceMagnitude,  // 힘의 크기
		BlinkDuration,   // 지속 시간
		false,           // IsAdditive (기존 속도에 더할 것인가? False = 덮어쓰기)
		nullptr,         // Strength Curve (없으면 상수 힘)
		ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, // 끝난 후 속도 유지 (관성)
		FVector::ZeroVector,
		0.0f,
		bEnableGravity   // 중력 적용 여부
	);

	if (RootMotionTask)
	{
		RootMotionTask->OnFinish.AddDynamic(this, &ThisClass::OnBlinkFinished);
		RootMotionTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UOWGA_Tracer_Blink::OnBlinkFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
