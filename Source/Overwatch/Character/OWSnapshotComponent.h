// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OWSnapshotComponent.generated.h"

USTRUCT(BlueprintType)
struct FOWRecallSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly)
	float Health = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float Timestamp = 0.0f;	// 서버 시간
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OVERWATCH_API UOWSnapshotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOWSnapshotComponent();
	
	// 3초 전의 가장 근접한 데이터를 찾아 반환합니다.
	UFUNCTION(BlueprintCallable)
	bool GetRecallTargetSnapshot(FOWRecallSnapshot& OutSnapshot);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 스냅샷 버퍼(원형 버퍼)
	TArray<FOWRecallSnapshot> SnapshotBuffer;

	// 스냅샷 옵션
	UPROPERTY(EditDefaultsOnly)
	float RecordInterval = 0.1f;	// 몇 초 단위로 기록할지
	
	float MaxHistoryTime = 3.0f;	// 최대 몇 초 기록할지
	float Accumulator = 0.0f;		// 누적 기록
};
