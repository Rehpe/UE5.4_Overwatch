// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;
/**
 *  OWGameplayTags
 * C++ 코드에서 자주 사용하는 네이티브 태그들을 캐싱해두고 전역에서 접근할 수 있게 해주는 static struct
 * FGameplayTag AirTag = FOWGameplayTags::Get().State_Movement_Air; 
 */

struct FOWGameplayTags
{
public:
	FORCEINLINE static const FOWGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// Ability
	FGameplayTag Ability_Action_Reload;	// 재장전
	
	// Event
	FGameplayTag GameplayEvent_Character_Death; // 캐릭터 사망
	FGameplayTag GameplayEvent_Ability_Reload_Refill; // 총알 재장전
	FGameplayTag GameplayEvent_Weapon_OutOfAmmo; // 탄약 0 -> 재장전 요청

	// State
	FGameplayTag State_Dead;			// 사망
	FGameplayTag State_Movement_Air;	// 캐릭터 공중에 뜸
	FGameplayTag State_Combat_Firing;	// 캐릭터 사격 중

	// GameplayCue
	FGameplayTag GameplayCue_Character_Death;	// 죽음

	// Data
	FGameplayTag Data_HealAmount;	// 힐량
	
	// Sound
	FGameplayTag Sound_Voice_Death;
	FGameplayTag Sound_Voice_Pain;
	FGameplayTag Sound_Voice_Jumping;
	FGameplayTag Sound_Voice_JumpEnd;
	FGameplayTag Sound_Voice_MeleeAttack;
	FGameplayTag Sound_Voice_HealPack;
	FGameplayTag Sound_Voice_HealthRestoredFriendly;
	FGameplayTag Sound_Voice_Respawn;

protected:
	// 태그 등록 헬퍼 함수
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FOWGameplayTags GameplayTags;
};