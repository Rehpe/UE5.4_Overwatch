// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "OWAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UOWAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
