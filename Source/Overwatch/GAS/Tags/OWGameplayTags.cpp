// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Tags/OWGameplayTags.h"
#include "GameplayTagsManager.h"

FOWGameplayTags FOWGameplayTags::GameplayTags;

void FOWGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	// ==== Ability
	GameplayTags.AddTag(GameplayTags.Ability_Action_Reload, "Ability.Action.Reload", "Ability.Action.Reload");
	GameplayTags.AddTag(GameplayTags.Ability_Action_MeleeAttack, "Ability.Action.MeleeAttack", "Ability.Action.MeleeAttack");

	// ==== Event
	GameplayTags.AddTag(GameplayTags.GameplayEvent_Character_Death, "GameplayEvent.Character.Death", "GameplayEvent.Character.Death");
	GameplayTags.AddTag(GameplayTags.GameplayEvent_Damage_Dealt, "GameplayEvent.Damage.Dealt", "GameplayEvent.Damage.Dealt");
	GameplayTags.AddTag(GameplayTags.GameplayEvent_Ability_Reload_Refill, "GameplayEvent.Ability.Reload.Refill", "GameplayEvent.Ability.Reload.Refill");
	GameplayTags.AddTag(GameplayTags.GameplayEvent_Ability_Tracer_Ult_ThrowBomb, "GameplayEvent.Ability.Tracer.Ult.ThrowBomb", "GameplayEvent.Ability.Tracer.Ult.ThrowBomb");
	GameplayTags.AddTag(GameplayTags.GameplayEvent_Weapon_OutOfAmmo, "GameplayEvent.Weapon.OutOfAmmo", "GameplayEvent.Weapon.OutOfAmmo");
	GameplayTags.AddTag(GameplayTags.GameplayEvent_Melee_Hit, "GameplayEvent.Melee.Hit", "GameplayEvent.Melee.Hit");
	GameplayTags.AddTag(GameplayTags.GameplayEvent_Regen_Stamina, "GameplayEvent.Regen.Stamina", "GameplayEvent.Regen.Stamina");
	
	// ==== State
	GameplayTags.AddTag(GameplayTags.State_Regen_Stamina, "State.Regen.Stamina", "State.Regen.Stamina");
	
	GameplayTags.AddTag(GameplayTags.State_Block_Input, "State.Block.Input", "State.Block.Input");
	GameplayTags.AddTag(GameplayTags.State_Block_Ability, "State.Block.Ability", "State.Block.Ability");

	GameplayTags.AddTag(GameplayTags.State_Dead, "State.Dead", "State.Dead");

	GameplayTags.AddTag(GameplayTags.State_Invulnerable, "State.Invulnerable", "State.Invulnerable");
	GameplayTags.AddTag(GameplayTags.State_Untargetable, "State.Untargetable", "State.Untargetable");
	
	GameplayTags.AddTag(GameplayTags.State_Movement_Air, "State.Movement.Air", "State.Movement.Air");
	GameplayTags.AddTag(GameplayTags.State_Combat_Firing, "State.Combat.Firing", "State.Combat.Firing");

	// ==== GameplayCue
	// 공용
	GameplayTags.AddTag(GameplayTags.GameplayCue_Character_WeaponFire, "GameplayCue.Character.WeaponFire", "GameplayCue.Character.WeaponFire");
	GameplayTags.AddTag(GameplayTags.GameplayCue_Character_Jump, "GameplayCue.Character.Jump", "GameplayCue.Character.Jump");
	GameplayTags.AddTag(GameplayTags.GameplayCue_Character_MeleeAttack, "GameplayCue.Character.MeleeAttack", "GameplayCue.Character.MeleeAttack");
	GameplayTags.AddTag(GameplayTags.GameplayCue_Character_Death, "GameplayCue.Character.Death", "GameplayCue.Character.Death");

	// Tracer
	GameplayTags.AddTag(GameplayTags.GameplayCue_Tracer_Blink, "GameplayCue.Tracer.Blink", "GameplayCue.Tracer.Blink");
	GameplayTags.AddTag(GameplayTags.GameplayCue_Tracer_Ult_Cast, "GameplayCue.Tracer.Ult.Cast", "GameplayCue.Tracer.Ult.Cast");
	GameplayTags.AddTag(GameplayTags.GameplayCue_Tracer_Ult_OnAttached, "GameplayCue.Tracer.Ult.OnAttached", "GameplayCue.Tracer.Ult.OnAttached");
	GameplayTags.AddTag(GameplayTags.GameplayCue_Tracer_Ult_Explode, "GameplayCue.Tracer.Ult.Explode", "GameplayCue.Tracer.Ult.Explode");

	// ===== Data
	GameplayTags.AddTag(GameplayTags.Data_Damage, "Data.Damage", "Data.Damage");
	GameplayTags.AddTag(GameplayTags.Data_Damage_Melee, "Data.Damage.Melee", "Data.Damage.Melee");
	GameplayTags.AddTag(GameplayTags.Data_Damage_WeaponFire, "Data.Damage.WeaponFire", "Data.Damage.WeaponFire");
	GameplayTags.AddTag(GameplayTags.Data_Damage_Tracer_Ult, "Data.Damage.Tracer.Ult", "Data.Damage.Tracer.Ult");
	
	GameplayTags.AddTag(GameplayTags.Data_HealAmount, "Data.HealAmount", "Data.HealAmount");
	
	// ==== Sound
	// SFX
	GameplayTags.AddTag(GameplayTags.Sound_SFX_MeleeAttack, "Sound.SFX.MeleeAttack", "Sound.SFX.MeleeAttack");
	
	// Voice
	GameplayTags.AddTag(GameplayTags.Sound_Voice_Death, "Sound.Voice.Death", "Sound.Voice.Death");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_Pain, "Sound.Voice.Pain", "Sound.Voice.Pain");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_Jumping, "Sound.Voice.Jumping", "Sound.Voice.Jumping");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_JumpEnd, "Sound.Voice.JumpEnd", "Sound.Voice.JumpEnd");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_MeleeAttack, "Sound.Voice.MeleeAttack", "Sound.Voice.MeleeAttack");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_HealPack, "Sound.Voice.HealPack", "Sound.Voice.HealPack");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_HealthRestoredFriendly, "Sound.Voice.HealthRestoredFriendly", "Sound.Voice.HealthRestoredFriendly");
	GameplayTags.AddTag(GameplayTags.Sound_Voice_Respawn, "Sound.Voice.Respawn", "Sound.Voice.Respawn");

	// UI
	GameplayTags.AddTag(GameplayTags.UI_Passive, "UI.Passive", "UI.Passive");
	GameplayTags.AddTag(GameplayTags.UI_Barrier, "UI.Barrier", "UI.Barrier");
	GameplayTags.AddTag(GameplayTags.UI_SkillSlot, "UI.SkillSlot", "UI.SkillSlot");
	GameplayTags.AddTag(GameplayTags.UI_Ult, "UI.Ult", "UI.Ult");
	GameplayTags.AddTag(GameplayTags.UI_Ammo, "UI.Ammo", "UI.Ammo");

	// Damage Modifier
	GameplayTags.AddTag(GameplayTags.Damage_Modifier_Headshot, "Damage.Modifier.Headshot", "Damage.Modifier.Headshot");
	
	//UE_LOG(LogTemp, Display, TEXT("[OWGameplayTags] Native Tags Initialized!"));
}

void FOWGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(ANSI_TO_TCHAR(TagComment)));
}
