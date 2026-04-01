// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OWCharacterBase.h"

#include "Overwatch.h"
#include "AbilitySystemComponent.h"
#include "Player/OWPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "OWAnimInstance.h"
#include "OWCombatComponent.h"
#include "OWHealthComponent.h"
#include "OWHeroData.h"
#include "OWHeroVoiceData.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/OWAbilitySet.h"
#include "GAS/OWAbilitySystemComponent.h"
#include "GAS/Attributes/OWAttributeSet_Base.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Input/OWEnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/OWPlayerController.h"
#include "Weapon/OWWeapon.h"
#include "Widget/OWUserWidget.h"


AOWCharacterBase::AOWCharacterBase()
{
 	//PrimaryActorTick.bCanEverTick = true;
	ASC = nullptr;
	AttributeSet_Base = nullptr;
	AttributeSet_Weapon = nullptr;
	AttributeSet_Skill = nullptr;
	
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
	FirstPersonMesh->SetLightAttachmentsAsGroup(true); 

	// 3p Mesh 안보이게, 그림자 안 보임
	GetMesh()->SetOwnerNoSee(true); 
	GetMesh()->CastShadow = false;
	//최적화 끄기 (서버도 애니메이션을 돌려야 함)
	GetMesh()->bEnableUpdateRateOptimizations = false;
	// 화면에 안 보여도 무조건 뼈를 갱신하고 Notify를 체크해라
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	// -- Movement --
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->GravityScale = 3.5f;
		GetCharacterMovement()->JumpZVelocity = 1000.0f;
		GetCharacterMovement()->AirControl = 1.0f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
		GetCharacterMovement()->MaxWalkSpeedCrouched = 300.0f;
		JumpMaxHoldTime = 0.0f;
		BaseEyeHeight = 70.f;
		CrouchedEyeHeight = 25.f;
	}
	
	// -- hp bar --
	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	
	HealthBarWidgetComp->SetupAttachment(RootComponent); 
	HealthBarWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f)); 
	
	HealthBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComp->SetDrawSize(FVector2D(150.0f, 40.0f));
	
	HealthBarWidgetComp->bOwnerNoSee = true;

	//  -- Component
	HealthComponent = CreateDefaultSubobject<UOWHealthComponent>(TEXT("HealthComponent"));
	CombatComponent = CreateDefaultSubobject<UOWCombatComponent>(TEXT("CombatComponent"));
}

// 데디서버환경에서는 서버에서만 호출됨
void AOWCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();  // ASC 초기화
	ApplyHeroData(); // 영웅 별 데이터 적용
	InitWidget();	// Hud, Hp바 초기화
	InitASCListeners();	// 태그 감시
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

void AOWCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// 클라이언트에서 ASC 초기화
	InitAbilityActorInfo();
	InitWidget();
	InitASCListeners();
	
	// BP에 미리 할당한 HeroData 갱신용
	if(HeroData)
	{
		OnRep_HeroData();
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

void AOWCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOWCharacterBase, HeroData);
	DOREPLIFETIME(AOWCharacterBase, Weapon);
}

void AOWCharacterBase::OnWeaponSet(AOWWeapon* NewWeapon)
{
	Weapon = NewWeapon;

	if (FirstPersonMesh && GetMesh())
	{
		Weapon->Equip(FirstPersonMesh, GetMesh());
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

	// HeroName
	if(HealthBarWidgetComp)
	{
		if (UOWUserWidget* HealthWidget = Cast<UOWUserWidget>(HealthBarWidgetComp->GetUserWidgetObject()))
		{
			HealthWidget->InitHeroName(HeroData->HeroName); 
		}
	}

	// SkillSet GAS
	if (HasAuthority()) // Late Join 대비 Authority 검사
	{
		if (ASC && HeroData->AbilitySet)
		{
			HeroData->AbilitySet->GiveToAbilitySystem(ASC, this);
		}
		
		if (HeroData->WeaponClass)
		{
			if(Weapon)
			{
				Weapon->Destroy();
				Weapon = nullptr;
			}
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this; // 무기 주인
			SpawnParams.Instigator = this; // 가해자 
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 무조건 소환

			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(HeroData->WeaponClass, GetTransform(), SpawnParams);
			Weapon = Cast<AOWWeapon>(SpawnedActor);
			if (Weapon)
			{
				Weapon->SetOwner(this);
				Weapon->Equip(FirstPersonMesh, GetMesh());	
			}
		}
	}
}

void AOWCharacterBase::OnRep_HeroData()
{
	ApplyHeroData();
	
	if (Weapon && FirstPersonMesh && GetMesh())
	{
		Weapon->Equip(FirstPersonMesh, GetMesh());
	}
}

void AOWCharacterBase::InitAttributes()
{
	if(!HasAuthority()) return;
	
	if (!ASC || !HeroData)
	{
		return;
	}

	// 이펙트 컨텍스트 생성 (누가 시전했는지 기록)
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	// Base 초기화
	if (HeroData->InitState_Base)
	{
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(HeroData->InitState_Base, 1.0f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
		}
	}

	// Weapon 초기화
	if (HeroData->InitState_Weapon)
	{
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(HeroData->InitState_Weapon, 1.0f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
		}
	}

	// Skill 초기화
	if (HeroData->InitState_Skill)
	{
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(HeroData->InitState_Skill, 1.0f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
		}
	}
}

void AOWCharacterBase::InitASCListeners()
{
	if (ASC)
	{
		// State.Dead 
		ASC->RegisterGameplayTagEvent(FOWGameplayTags::Get().State_Dead, EGameplayTagEventType::NewOrRemoved)
		   .AddUObject(this, &AOWCharacterBase::OnDeathTagChanged);
	}
}

void AOWCharacterBase::OnDeathTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// 태그가 추가되었다면
	if (NewCount > 0)
	{
		OnDeathStarted();
	}
}

void AOWCharacterBase::OnHidden(bool bIsHidden)
{
	SetActorHiddenInGame(bIsHidden); // 본체 숨기기

	// 본체에 붙은 하위 액터 숨기기
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors); 
	for (AActor* AttachedActor : AttachedActors)
	{
		if (AttachedActor)
		{
			AttachedActor->SetActorHiddenInGame(bIsHidden);
		}
	}
	
	HealthBarWidgetComp->SetVisibility(!bIsHidden); // hp바 숨기기
}

void AOWCharacterBase::OnWidgetHidden(bool bIsHidden)
{
	HealthBarWidgetComp->SetVisibility(!bIsHidden); // hp바 숨기기
}

USoundBase* AOWCharacterBase::GetVoice(FGameplayTag VoiceTag) const
{
	if (!HeroData || !HeroData->VoiceData)
		return nullptr;

	//맵에서 태그로 사운드 검색
	if (const TObjectPtr<USoundBase>* FoundSound = HeroData->VoiceData->VoiceMap.Find(VoiceTag))
	{
		return *FoundSound; 
	}
	return nullptr;
}

UOWHeroData* AOWCharacterBase::GetHeroData() const
{
	if(!ASC) return nullptr;
	return HeroData;
}

float AOWCharacterBase::GetHealth() const
{
	if(!ASC)
		return 0.f;
	
	return GetAbilitySystemComponent()->GetNumericAttribute(UOWAttributeSet_Base::GetHealthAttribute());
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
	// 차단 태그가 있다면 이동 입력 무시
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(FOWGameplayTags::Get().State_Block_Input))
	{
		return; 
	}
	
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
	// 차단 태그가 있다면 방향 전환 무시
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(FOWGameplayTags::Get().State_Block_Input))
	{
		return; 
	}
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AOWCharacterBase::InitAbilityActorInfo()
{
	AOWPlayerState* PS = GetPlayerState<AOWPlayerState>();
	if(PS)
	{
		// ASC 받아옴
		ASC = PS->GetOWAbilitySystemComponent();
		if(ASC)
		{
			ASC->InitAbilityActorInfo(PS, this); // ASC 초기화

			if (HealthComponent)
			{
				HealthComponent->InitializeWithAbilitySystem(ASC);
			}
			if (CombatComponent)
			{
				CombatComponent->InitializeWithAbilitySystem(ASC);
			}
			
			if (GetMesh() && GetMesh()->GetAnimInstance())
			{
				// AnimInstance에 ASC 전달
				UOWAnimInstance* AnimInst = Cast<UOWAnimInstance>(GetMesh()->GetAnimInstance());
				if (AnimInst)
				{
					AnimInst->InitASC(ASC);
				}
			}
		}

		// AS 3종 받아오고 초기화
		AttributeSet_Base = PS->GetAttributeSet_Base();
		AttributeSet_Weapon = PS->GetAttributeSet_Weapon();
		AttributeSet_Skill = PS->GetAttributeSet_Skill();

		InitAttributes();
	}
}

void AOWCharacterBase::InitWidget()
{
	if (!HealthComponent) return;

	if (HealthBarWidgetComp && HealthBarWidgetComp->GetUserWidgetObject())
	{
		if (UOWUserWidget* HealthWidget = Cast<UOWUserWidget>(HealthBarWidgetComp->GetUserWidgetObject()))
		{
			HealthWidget->InitWidgetWithHealthComp(HealthComponent);
          
			if (HeroData) HealthWidget->InitHeroName(HeroData->HeroName);
		}
	}
	if (IsLocallyControlled())
	{
		if (AOWPlayerController* PC = Cast<AOWPlayerController>(GetController()))
		{
			PC->SetupHUD(HealthComponent, CombatComponent); 
		}
	}
}

void AOWCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!ASC) return;
	
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

void AOWCharacterBase::OnDeathStarted()
{
	// 캡슐 콜리전 해제
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 이동 및 관성 정지
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
		
	if (USkeletalMeshComponent* MyMesh = GetMesh())
	{
		// 콜리전 프로파일 변경
		MyMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		MyMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
            
		// 랙돌 활성화
		MyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MyMesh->SetCollisionProfileName(FName("Ragdoll"));
		MyMesh->SetSimulatePhysics(true);
	}

	// 무기 삭제
	if (Weapon) 
	{
		Weapon->Destroy();
		Weapon = nullptr;
	}
}

class UAbilitySystemComponent* AOWCharacterBase::GetAbilitySystemComponent() const
{
	return ASC.Get();
}
