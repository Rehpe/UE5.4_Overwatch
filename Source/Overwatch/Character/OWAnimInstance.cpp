// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OWAnimInstance.h"

#include "OWCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/OWAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GAS/Tags/OWGameplayTags.h"

UOWAnimInstance::UOWAnimInstance()
	: GroundSpeed(0.f)
	, Direction(0.f)
	, bShouldMove(false)
	, bIsFalling(false)
	, bIsFiring(false)
	, bIsCrouching(false)
{
}

void UOWAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AOWCharacterBase>(TryGetPawnOwner());
	
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
		UAbilitySystemComponent* GenericASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character);
		ASC = Cast<UOWAbilitySystemComponent>(GenericASC);
	}
}

void UOWAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character || !MovementComponent)
		return;

	// 속도 계산
	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	// 움직임 판단 (입력이 있거나, 속도가 있을 때)
	bool bHasInput = MovementComponent->GetCurrentAcceleration().SizeSquared() > 0.0f;
	bShouldMove = (GroundSpeed > 3.0f) || bHasInput;

	// 공중 판정
	bIsFalling = MovementComponent->IsFalling();

	// 앉기 판정
	bIsCrouching = Character->bIsCrouched;

	// 방향 판정
	if (bShouldMove)
	{
		// 캐릭터의 Rotation을 이용해 속도 벡터를 역회전(Unrotate) (월드 -> 로컬 변환)
		FVector LocalVelocity = Character->GetActorRotation().UnrotateVector(Velocity);

		// 로컬 X, Y를 이용해 각도 구하기 
		float DirectionRad = FMath::Atan2(LocalVelocity.Y, LocalVelocity.X);

		// Radian -> Degree
		Direction = FMath::RadiansToDegrees(DirectionRad);
	}
	else
	{
		Direction = 0.0f;
	}

	if (ASC)
	{
		// 사격 중 판정
		FGameplayTag FiringTag = FOWGameplayTags::Get().State_Combat_Firing;
		bIsFiring = ASC->HasMatchingGameplayTag(FiringTag);
	}
}
