// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GCN/OWGCN_Tracer_Recall.h"

#include "Camera/CameraComponent.h"
#include "Character/OWCharacterBase.h"
#include "Character/OWSnapshotComponent.h"
#include "Kismet/GameplayStatics.h"

AOWGCN_Tracer_Recall::AOWGCN_Tracer_Recall()
	: bIsLocallyControlled(false)
	, ElapsedTime(0.f)
	, RecallDuration(0.9f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; 
	bAutoDestroyOnRemove = true; // 완료 후 깔끔하게 메모리 해제
}

bool AOWGCN_Tracer_Recall::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	bool bResult = Super::OnActive_Implementation(MyTarget, Parameters);

	TargetCharacter = Cast<AOWCharacterBase>(MyTarget);
	if (!TargetCharacter) return bResult;

	// 1p, 3p 모두 위젯 숨김
	TargetCharacter->OnWidgetHidden(true);
	
	bIsLocallyControlled = TargetCharacter->IsLocallyControlled();
	ElapsedTime = 0.0f;
	
	if (bIsLocallyControlled)
	{
		// *** 1p ***
		// 스냅샷 준비
		UOWSnapshotComponent* SnapshotComp = TargetCharacter->FindComponentByClass<UOWSnapshotComponent>();
		if (SnapshotComp)
		{
			Snapshots = SnapshotComp->GetSnapshotBuffer();
		}
		TargetCamera = TargetCharacter->FindComponentByClass<UCameraComponent>();
		if (TargetCamera)
		{
			// 기존 카메라 변수 저장
			OriginCameraLocation = TargetCamera->GetRelativeLocation();
			OriginCameraRotation = TargetCamera->GetRelativeRotation();
			
			// 부모 이동에 영향받지 않도록 월드 좌표 기준으로 분리
			TargetCamera->SetUsingAbsoluteLocation(true);
			TargetCamera->SetUsingAbsoluteRotation(true);
		}
		// PostProcess
		if (PP_RecallMaterial && TargetCamera)
		{
			// Dynamic Material 생성
			RecallMID = UMaterialInstanceDynamic::Create(PP_RecallMaterial, this);
    
			// 현재 카메라에 PP효과 Blend
			TargetCamera->PostProcessSettings.AddBlendable(RecallMID, 1.0f);
		}
		// SFX
		if (RecallSFX_1P && RecallVoice_1P)
		{
			UGameplayStatics::PlaySound2D(this, RecallSFX_1P);
			UGameplayStatics::PlaySound2D(this, RecallVoice_1P);
		}
		SetActorTickEnabled(true);
	}
	else
	{
		// 3p
		TargetCharacter->OnHidden(true);	// 투명화처리
		if (RecallStartFX_3P)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RecallStartFX_3P, TargetCharacter->GetActorTransform());
		}
	}

	return bResult;
}

bool AOWGCN_Tracer_Recall::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (TargetCharacter)
	{
		// 1p, 3p 모두 위젯 해제
		TargetCharacter->OnWidgetHidden(false);
		
		if (bIsLocallyControlled)
		{
			// 1p
			// 1p 스냅샷
			SetActorTickEnabled(false);
			if (TargetCamera)
			{
				TargetCamera->SetWorldLocationAndRotation(TargetCharacter->GetActorLocation(), TargetCharacter->GetActorRotation());
				
				// 역행 종료 시 카메라를 다시 캡슐에 종속
				TargetCamera->SetUsingAbsoluteLocation(false);
				TargetCamera->SetUsingAbsoluteRotation(false);
				
				// 기존 카메라 위치로 초기화
				TargetCamera->SetRelativeLocationAndRotation(OriginCameraLocation, OriginCameraRotation);
			}
			// PostProcess
			if (TargetCamera && bIsLocallyControlled)
			{
				// 등록한 모든 PP Blendable 비우기
				TargetCamera->PostProcessSettings.WeightedBlendables.Array.Empty();
			}
		}
		else
		{
			// 3p
			TargetCharacter->OnHidden(false); // 투명화 해제
			if (RecallEndFX_3P)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RecallEndFX_3P, TargetCharacter->GetActorTransform());
			}
		}
	}

	return Super::OnRemove_Implementation(MyTarget, Parameters);
}

void AOWGCN_Tracer_Recall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1p에서만 작동
	if (!bIsLocallyControlled || !TargetCharacter || !TargetCamera || Snapshots.Num() < 2) return;
	
	ElapsedTime += DeltaTime;

	// 진행률 백분위
	float Alpha = FMath::Clamp(ElapsedTime / RecallDuration, 0.0f, 1.0f);

	// 배열의 마지막 인덱스(최신)
	int32 LastIndex = Snapshots.Num() - 1;
	// 소수점 인덱스(LastIndex->0번 인덱스까지 백분위에 따른 소수점 인덱스)
	float FloatIndex = FMath::Lerp((float)LastIndex, 0.0f, Alpha); 

	// 소수점 인덱스를 올림/버림하여 두 인덱스값 계산
	int32 IndexA = FMath::CeilToInt(FloatIndex); 
	int32 IndexB = FMath::FloorToInt(FloatIndex); 
    
	// 두 인덱스값 사이의 보간 비율
	float LerpFraction = FloatIndex - (float)IndexB;

	if (Snapshots.IsValidIndex(IndexA) && Snapshots.IsValidIndex(IndexB))
	{
		// 1. Snapshot 기반 캐릭터 본체의 위치/회전값 보간
		FVector CharacterLocation = FMath::Lerp(Snapshots[IndexB].Location, Snapshots[IndexA].Location, LerpFraction);

		// 쿼터니언으로 보간
		FQuat QuatA = Snapshots[IndexA].ControlRotation.Quaternion();
		FQuat QuatB = Snapshots[IndexB].ControlRotation.Quaternion();
		FRotator CharacterRotation = FQuat::Slerp(QuatB, QuatA, LerpFraction).Rotator();
		
		// 2. 보간된 캐릭터 본체의 Transform(좌표계) 생성
		FTransform CharacterTransform(CharacterRotation, CharacterLocation);
		
		// 3. 캐릭터 기준점에 카메라 오프셋을 더해 최종 위치/회전값 계산
		FVector FinalLocation = CharacterTransform.TransformPosition(OriginCameraLocation);
		FRotator FinalRotation = CharacterTransform.TransformRotation(OriginCameraRotation.Quaternion()).Rotator();
		if (APlayerController* PC = Cast<APlayerController>(TargetCharacter->GetController()))
		{
			// 컨트롤러의 시선을 보간된 회전값으로 강제 고정
			PC->SetControlRotation(CharacterRotation);
		}
		
		// 5. 계산된 최종 위치로 카메라 이동
		TargetCamera->SetWorldLocationAndRotation(FinalLocation, FinalRotation);
	}

	// PostProcess
	if (RecallMID)
	{
		float EffectAlpha = FMath::Sin(Alpha * PI);
		float EffectIntense = 3.0f;
		RecallMID->SetScalarParameterValue(FName("DistortionIntensity"), EffectAlpha * EffectIntense);
	}
}
