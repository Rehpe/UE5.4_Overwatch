// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OWCharacterBase.h"

#include "Overwatch.h"
#include "AbilitySystemComponent.h"
#include "Player/OWPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "OWHeroData.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/OWAbilitySet.h"
#include "GAS/OWAbilitySystemComponent.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Input/OWEnhancedInputComponent.h"
#include "Weapon/OWWeapon.h"


AOWCharacterBase::AOWCharacterBase()
{
 	//PrimaryActorTick.bCanEverTick = true;
	ASC = nullptr;
	AttributeSet = nullptr;

	// -- 1p Camera Setting --
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	// 마우스 돌리면 시선도 같이 돌아가게
	FirstPersonCamera->bUsePawnControlRotation = true; 
	// 트레이서 눈높이 설정 (캡슐 중앙 기준 위로 올림)
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f)); // BaseEyeHeight 고려

	// -- Mesh Setting --
	// 1p Mesh Setting
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	// 1p Mesh 보이게, 그림자 안보임
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->CastShadow = false; 
	FirstPersonMesh->bCastDynamicShadow = false;
	// 3p Mesh 안보이게, 그림자 보임
	GetMesh()->SetOwnerNoSee(true); 
	GetMesh()->bCastHiddenShadow = true;

	// -- Movement --
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->GravityScale = 3.5f;
		GetCharacterMovement()->JumpZVelocity = 1000.0f;
		GetCharacterMovement()->AirControl = 1.0f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		JumpMaxHoldTime = 0.0f;
	}
}

// 데디서버환경에서는 서버에서만 호출됨
void AOWCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();  // ASC 초기화
	ApplyHeroData(); // 영웅 별 데이터 적용
}

void AOWCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// IMC 등록
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AOWCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UOWEnhancedInputComponent* OWEIC = CastChecked<UOWEnhancedInputComponent>(PlayerInputComponent);

	if (InputConfig)
	{
		// 이동, 시점 같은 기본 입력은 연속적이므로 GAS 없이 여기서 처리한다
		OWEIC->BindAction(InputConfig->Move, ETriggerEvent::Triggered, this, &AOWCharacterBase::Input_Move);
		OWEIC->BindAction(InputConfig->Look, ETriggerEvent::Triggered, this, &AOWCharacterBase::Input_Look);

		// [GAS Input] Config 내의 GA를 태그와 묶어 바인딩
		TArray<uint32> BindHandles;
		OWEIC->BindAbilityActions(InputConfig, 
								this, 
								&AOWCharacterBase::Input_AbilityInputTagPressed, 
								&AOWCharacterBase::Input_AbilityInputTagReleased,
								BindHandles
		);
	}
}

void AOWCharacterBase::ApplyHeroData()
{
	if (!HeroData) return;

	// 원래는 AsyncLoad(비동기)가 정석이지만, 
	// 지금은 구현 복잡도를 낮추기 위해 LoadSynchronous(동기)로 "Soft Ptr 사용"만 구현합니다.
	// 나중에 이 함수 내부만 StreamableManager 코드로 바꾸면 최적화 끝입니다.

	// 1p Mesh
	USkeletalMesh* LoadedMesh1P = HeroData->Mesh1P.LoadSynchronous();
	if (LoadedMesh1P)
	{
		FirstPersonMesh->SetSkeletalMesh(LoadedMesh1P);
	}
	// 1p Anim
	UClass* LoadedAnim1P = HeroData->AnimClass1P.LoadSynchronous();
	if (LoadedAnim1P)
	{
		FirstPersonMesh->SetAnimInstanceClass(LoadedAnim1P);
	}

	// 3p Mesh
	USkeletalMesh* LoadedMesh3P = HeroData->Mesh3P.LoadSynchronous();
	if (LoadedMesh3P)
	{
		GetMesh()->SetSkeletalMesh(LoadedMesh3P);
	}

	// 3p Anim
	UClass* LoadedAnim3P = HeroData->AnimClass3P.LoadSynchronous();
	if (LoadedAnim3P)
	{
		GetMesh()->SetAnimInstanceClass(LoadedAnim3P);
	}

	// SkillSet GAS
	if (HasAuthority() && ASC)	// Late Join 대비 Authority 검사
	{
		if (HeroData && HeroData->AbilitySet)
		{
			HeroData->AbilitySet->GiveToAbilitySystem(ASC, this);
		}
	}

	// Weapon
	if (Weapon)
	{
		Weapon->Destroy();
		Weapon = nullptr;
	}
	if (HeroData->WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;      // 무기 주인
		SpawnParams.Instigator = this; // 가해자 
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 무조건 소환
		
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(HeroData->WeaponClass, GetTransform(), SpawnParams);
		Weapon = Cast<AOWWeapon>(SpawnedActor);
		if (Weapon)
		{
			Weapon->Equip(FirstPersonMesh, GetMesh());
		}
	}
}

void AOWCharacterBase::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (ASC)
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AOWCharacterBase::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (ASC)
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

void AOWCharacterBase::Input_Move(const struct FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AOWCharacterBase::Input_Look(const struct FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AOWCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// 클라이언트에서 ASC 초기화
	InitAbilityActorInfo();
}

void AOWCharacterBase::InitAbilityActorInfo()
{
	AOWPlayerState* PS = GetPlayerState<AOWPlayerState>();
	if(PS)
	{
		ASC = PS->GetOWAbilitySystemComponent();
		if(ASC)
		{
			ASC->InitAbilityActorInfo(PS, this); // ASC 초기화
		}
		AttributeSet = PS->GetAttributeSet();
	}
}

void AOWCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!ASC) return;

	// 1. 기존 이동 관련 태그 싹 지우기 (초기화)
	// (미리 정의해둔 State.Movement 부모 태그 하위의 모든 태그 제거)
	FGameplayTag AirTag = FOWGameplayTags::Get().State_Movement_Air;

	if (GetCharacterMovement()->IsFalling())
	{
		ASC->AddLooseGameplayTag(AirTag);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(AirTag);
	}
}

class UAbilitySystemComponent* AOWCharacterBase::GetAbilitySystemComponent() const
{
	return ASC.Get();
}
