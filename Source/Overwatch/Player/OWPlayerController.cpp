// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OWPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Widget/OWUserWidget.h"

AOWPlayerController::AOWPlayerController()
{
}

void AOWPlayerController::SetupHUD(class UOWHealthComponent* HealthComp, class UOWCombatComponent* CombatComp)
{
	// HUD가 아직 생성되지 않았다면 생성
	if (!HeroHUDInstance && HeroHUDClass)
	{
		HeroHUDInstance = CreateWidget<UOWUserWidget>(this, HeroHUDClass);
		if (HeroHUDInstance)
		{
			HeroHUDInstance->AddToViewport();
		}
	}

	// 생성된 HUD에 컴포넌트 연결 
	if (HeroHUDInstance)
	{
		if (HealthComp)
		{
			HeroHUDInstance->InitWidgetWithHealthComp(HealthComp);
		}
		
		if (CombatComp)
		{
			HeroHUDInstance->InitWidgetWithCombatComp(CombatComp);
		}
	}
}