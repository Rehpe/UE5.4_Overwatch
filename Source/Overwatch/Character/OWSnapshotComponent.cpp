
#include "Character/OWSnapshotComponent.h"

#include "OWCharacterBase.h"

UOWSnapshotComponent::UOWSnapshotComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UOWSnapshotComponent::BeginPlay()
{
	Super::BeginPlay();
	SnapshotBuffer.Reserve(40);
}

void UOWSnapshotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AOWCharacterBase* OWCharacter = Cast<AOWCharacterBase>(GetOwner());
	if (!OWCharacter) return;
	
	// Server : 검증용, 본인 Client(AutonomousProxy) : 연출용 => 이 경우에만 기록
	ENetRole LocalRole = OWCharacter->GetLocalRole();
	if(LocalRole != ROLE_Authority && LocalRole != ROLE_AutonomousProxy) return;
	
	// 흐른 시간 누적
	Accumulator += DeltaTime;

	// 흐른 시간이 기록 주기보다 커지면
	if (Accumulator >= RecordInterval)
	{
		// 남은 시간을 흐른 시간에 넘김
		Accumulator -= RecordInterval;

		// 신규 스냅샷 생성 및 데이터 채우기
		FOWRecallSnapshot NewSnapshot;
		NewSnapshot.Location = OWCharacter->GetActorLocation();
		NewSnapshot.Rotation = OWCharacter->GetActorRotation();
		NewSnapshot.Health = OWCharacter->GetHealth(); 
		NewSnapshot.Timestamp = GetWorld()->GetTimeSeconds();

		// 배열에 추가
		SnapshotBuffer.Add(NewSnapshot);

		// 3초가 경과한 오래된 데이터 제거
		float CurrentTime = NewSnapshot.Timestamp;
		while (SnapshotBuffer.Num() > 0 && (CurrentTime - SnapshotBuffer[0].Timestamp) > MaxHistoryTime)
		{
			SnapshotBuffer.RemoveAt(0);
		}
	}
}


bool UOWSnapshotComponent::GetRecallTargetSnapshot(FOWRecallSnapshot& OutSnapshot)
{
	// 기록된 데이터가 없다면 실패
	if (SnapshotBuffer.Num() == 0)
	{
		return false;
	}
	// 0번 인덱스 반환 -> 3초 이전이라도 가장 오래된 인덱스를 반환한다
	OutSnapshot = SnapshotBuffer[0];
	return true;
}
