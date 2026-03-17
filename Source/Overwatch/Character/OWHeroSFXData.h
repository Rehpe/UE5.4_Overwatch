// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "OWHeroSFXData.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWHeroSFXData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, TObjectPtr<USoundBase>> SFXMap;

		
#if WITH_EDITOR
	// 에디터에서만 보임 / 누르면 태그 자동 등록
	UFUNCTION(CallInEditor)
	void FillRequiredTags();
#endif
	
};
