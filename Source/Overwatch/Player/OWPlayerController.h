// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OWPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API AOWPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOWPlayerController();
	
	// HUD 초기화
	UFUNCTION(BlueprintCallable)
	void SetupHUD(class UOWHealthComponent* HealthComp, class UOWCombatComponent* CombatComp);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UOWUserWidget> HeroHUDClass;

	// 실제 HUD
	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UOWUserWidget> HeroHUDInstance;
};
