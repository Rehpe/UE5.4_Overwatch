// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "OWGCN_Character_Death.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWGCN_Character_Death : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UOWGCN_Character_Death();

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;
};
