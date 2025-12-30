// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "OWAbilitySet.generated.h"

/**
 * [구조체] 어빌리티 클래스 + 태그
 */
USTRUCT(BlueprintType)
struct FOWAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	// GA 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayAbility> Ability = nullptr;
	
	// Tag
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

UCLASS()
class OVERWATCH_API UOWAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	// 부여할 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FOWAbilitySet_GameplayAbility> GrantedGameplayAbilities;
	
	// 모든 어빌리티 목록을 ASC에 등록
	void GiveToAbilitySystem(class UOWAbilitySystemComponent* OWASC, UObject* SourceObject = nullptr) const;
};
