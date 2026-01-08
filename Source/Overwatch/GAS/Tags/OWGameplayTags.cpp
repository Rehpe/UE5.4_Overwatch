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

	GameplayTags.AddTag(GameplayTags.Event_Character_Death, "Event.Character.Death", "Event.Character.Death");

	GameplayTags.AddTag(GameplayTags.State_Movement_Air, "State.Movement.Air", "State.Movement.Air");
	GameplayTags.AddTag(GameplayTags.State_Dead, "State.Dead", "State.Dead");

	UE_LOG(LogTemp, Display, TEXT("[OWGameplayTags] Native Tags Initialized!"));
}

void FOWGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(ANSI_TO_TCHAR(TagComment)));
}
