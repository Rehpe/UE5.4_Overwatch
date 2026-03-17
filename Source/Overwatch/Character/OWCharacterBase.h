// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "OWCharacterBase.generated.h"

struct FGameplayTag;
struct FInputActionValue;

UCLASS()
class OVERWATCH_API AOWCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOWCharacterBase();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override; // 데디서버환경에서는 서버에서만 호출됨
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnWeaponSet(class AOWWeapon* NewWeapon);
	
protected:
	// -- Mesh --
	UPROPERTY(ReplicatedUsing = OnRep_HeroData, BlueprintReadOnly, Category = "Data")
	TObjectPtr<class UOWHeroData> HeroData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> FirstPersonCamera;	// 1인칭 카메라
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USkeletalMeshComponent> FirstPersonMesh; //1인칭 메쉬
	
	UFUNCTION()
	void OnRep_HeroData();

	// HeroData의 데이터 에셋을 읽어 적용
	void ApplyHeroData();
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class AOWWeapon> Weapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<class UOWAttributeSet_Base> AttributeSet_Base;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<class UOWAttributeSet_Weapon> AttributeSet_Weapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<class UOWAttributeSet_Skill> AttributeSet_Skill;
	
	// 영웅별 AttributeSet 3종을 초기화
	virtual void InitAttributes();

	// 각종 상태 태그 변경 감시
	virtual void InitASCListeners();

	// Death 태그 변경 여부 감시 
	virtual void OnDeathTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

public:
	FORCEINLINE TObjectPtr<class USkeletalMeshComponent> GetFirstPersonMesh() const { return FirstPersonMesh; }
	
	FORCEINLINE TObjectPtr<AOWWeapon> GetWeapon() const { return Weapon; }

	UFUNCTION(BlueprintPure)
	USoundBase* GetVoice(FGameplayTag VoiceTag) const;

	UFUNCTION(BlueprintPure)
	UOWHeroData* GetHeroData() const;
	
	UFUNCTION(BlueprintPure)
	float GetHealth() const;
	
protected:
	// -- Input --
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UOWInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

protected:
	// -- GAS --
	virtual void OnRep_PlayerState() override;
	void InitAbilityActorInfo();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UOWAbilitySystemComponent> ASC;

protected:
	// -- Movement --
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

public:	
	//virtual void Tick(float DeltaTime) override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
