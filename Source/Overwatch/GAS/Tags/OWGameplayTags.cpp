// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Tags/OWGameplayTags.h"
#include "GameplayTagsManager.h"

FOWGameplayTags FOWGameplayTags::GameplayTags;

void FOWGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	// -----------------------------------------------------------------------
	// [등록] 태그와 변수 연결
	// -----------------------------------------------------------------------

	// Event
	GameplayTags.AddTag(GameplayTags.Event_Character_Death, "Event.Character.Death", "Event.Character.Death");

	// State
	GameplayTags.AddTag(GameplayTags.State_Movement_Air, "State.Movement.Air", "State.Movement.Air");
	GameplayTags.AddTag(GameplayTags.State_Dead, "State.Dead", "State.Dead");

	// GameplayCue
	GameplayTags.AddTag(GameplayTags.GameplayCue_Character_Death, "GameplayCue.Character.Death", "GameplayCue.Character.Death");

	// Sound
	GameplayTags.AddTag(GameplayTags.Sound_Voice_Death, "Sound.Voice.Death", "Sound.Voice.Death");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_Pain, "Sound.Voice.Pain", "Sound.Voice.Pain");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_Jumping, "Sound.Voice.Jumping", "Sound.Voice.Jumping");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_JumpEnd, "Sound.Voice.JumpEnd", "Sound.Voice.JumpEnd");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_MeleeAttack, "Sound.Voice.MeleeAttack", "Sound.Voice.MeleeAttack");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_HealPack, "Sound.Voice.HealPack", "Sound.Voice.HealPack");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_HealthRestoredFriendly, "Sound.Voice.HealthRestoredFriendly", "Sound.Voice.HealthRestoredFriendly");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_Respawn, "Sound.Voice.Respawn", "Sound.Voice.Respawn");
	
	UE_LOG(LogTemp, Display, TEXT("[OWGameplayTags] Native Tags Initialized!"));
}

void FOWGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(ANSI_TO_TCHAR(TagComment)));
}
