// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OWHealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOWHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERWATCH_API IOWHealthInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	UOWHealthComponent* GetOWHealthComponent() const;
};
