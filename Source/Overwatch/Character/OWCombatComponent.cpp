// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OWCombatComponent.h"

#include "AbilitySystemComponent.h"
#include "GAS/Attributes/OWAttributeSet_Base.h"
#include "GAS/Attributes/OWAttributeSet_Skill.h"
#include "GAS/Attributes/OWAttributeSet_Weapon.h"
#include "Character/OWHeroData.h"
#include "GAS/OWAbilitySet.h"
#include "GAS/GA/OWGameplayAbility.h"
#include "GAS/Tags/OWGameplayTags.h"

struct FOWAbilitySet_GameplayAbility;

UOWCombatComponent::UOWCombatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UOWCombatComponent::InitializeWithAbilitySystem(class UAbilitySystemComponent* InASC)
{
	if (!InASC) return;
	ASC = InASC;

	BaseAtt = InASC->GetSet<UOWAttributeSet_Base>();
	WeaponAtt = InASC->GetSet<UOWAttributeSet_Weapon>();
	SkillAtt = InASC->GetSet<UOWAttributeSet_Skill>();
	
	if (WeaponAtt)
		InASC->GetGameplayAttributeValueChangeDelegate(WeaponAtt->GetAmmoAttribute()).AddUObject(this, &ThisClass::HandleAmmoChanged);
	if (BaseAtt)
		InASC->GetGameplayAttributeValueChangeDelegate(BaseAtt->GetUltimateChargeAttribute()).AddUObject(this, &ThisClass::HandleUltChanged);
	if (SkillAtt)
	{
		InASC->GetGameplayAttributeValueChangeDelegate(SkillAtt->GetStaminaAttribute()).AddUObject(this, &ThisClass::HandleStaminaChanged);
		InASC->GetGameplayAttributeValueChangeDelegate(SkillAtt->GetBarrierAttribute()).AddUObject(this, &ThisClass::HandleBarrierChanged); 
	}
}

void UOWCombatComponent::GetHeroSkillLayout(const class UOWHeroData* HeroData, TArray<FOWSkillUIData>& OutSkillData,
	FOWSkillUIData& OutUltData)
{
	if (!HeroData || !HeroData->AbilitySet || !ASC) return;

	OutSkillData.Empty();
	SlotTagAttributeMap.Empty();

	// 가중치 : UI 태그 
	TArray<TPair<int32, FOWSkillUIData>> SkillPairs;

	// HeroData의 AS 내의 모든 GA를 대상으로 UI 태그가 있는 GA만 찾아온다
	for (const FOWAbilitySet_GameplayAbility& AbilityInfo : HeroData->AbilitySet->GrantedGameplayAbilities)
	{
		const UOWGameplayAbility* AbilityCDO = Cast<UOWGameplayAbility>(AbilityInfo.Ability.GetDefaultObject());
		if (!AbilityCDO) continue;

		FOWSkillUIData NewUIData;
		NewUIData.InputTag = AbilityInfo.InputTag; // 스킬실행 명령어 (ex. InputTag.Shift)
		NewUIData.SkillIcon = AbilityCDO->SkillIcon;
		
		FGameplayTag SlotTag = AbilityCDO->UISlotTag; // HUD 어느 칸에 그려져야 할지 위치 (ex. UI.SkillSlots)
		
		// Ult
		if (SlotTag.MatchesTagExact(FOWGameplayTags::Get().UI_Ult))
		{
			OutUltData = NewUIData;
			continue;
		}
		
		// SkillSlot
		if (SlotTag.MatchesTagExact(FOWGameplayTags::Get().UI_SkillSlot))
		{
			// 스킬 타입(쿨다운/스택형)에 따라 분류

			// 스택형
			if (AbilityCDO->MaxCooldownStacks > 1) 
			{
				NewUIData.bIsChargeSkill = true;
				
				// 해당 스킬이 사용하는 CostGE 파악
				UGameplayEffect* CostGE = AbilityCDO->GetCostGameplayEffect();
				if (CostGE && CostGE->Modifiers.Num() > 0)
				{
					FGameplayAttribute StaminaAttr = CostGE->Modifiers[0].Attribute;
					FGameplayAttribute MaxStamina;
					FGameplayAttribute RegenPeriod;
					FGameplayTag RegenTag;
					UOWAttributeSet_Skill::GetStaminaData(StaminaAttr, MaxStamina, RegenPeriod, RegenTag);

					// 스태미너 값이 변경될 경우 어떤 스킬인지 InputTag로 찾기 위해 맵에 저장
					SlotTagAttributeMap.Add(NewUIData.InputTag, StaminaAttr);

					// 리젠 중 태그 여부로 Regen 게이지 감시
					ASC->RegisterGameplayTagEvent(RegenTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
						this, &ThisClass::HandleRegenTagChanged, NewUIData.InputTag, RegenPeriod);
				}
			}
			else // 일반형
			{
				// Cooldown 태그 여부로 Cooldown 감시
				const FGameplayTagContainer* CooldownTags = AbilityCDO->GetCooldownTags();
				if (CooldownTags && CooldownTags->Num() > 0)
				{
					ASC->RegisterGameplayTagEvent(CooldownTags->First(), EGameplayTagEventType::NewOrRemoved).AddUObject(
						this, &ThisClass::HandleCooldownTagChanged, NewUIData.InputTag);
				}
			}
			
			// 위젯 위치 가중치 / InputTag 맵에 저장
			SkillPairs.Emplace(AbilityCDO->UISortOrder, NewUIData);
		}
	}

	// UI배치 가중치에 따라 정렬
	SkillPairs.Sort([](const auto& A, const auto& B) {
		return A.Key < B.Key;
	});

	// 정렬 완료된 Pair에서 InputTag만 담는다
	for (const auto& Pair : SkillPairs)
	{
		OutSkillData.Add(Pair.Value);
	}
}

void UOWCombatComponent::HandleAmmoChanged(const struct FOnAttributeChangeData& Data)
{
	float MaxValue = WeaponAtt ? WeaponAtt->GetMaxAmmo() : 0.f;
	OnAttributeChanged.Broadcast(FOWGameplayTags::Get().UI_Ammo, Data.OldValue, Data.NewValue, MaxValue);

	// 탄약이 0이 되는 순간 재장전 이벤트 발생
	if (Data.NewValue <= 0.0f && Data.OldValue > 0.0f)
	{
		if (ASC)
		{
			FGameplayEventData Payload;
			Payload.EventTag = FOWGameplayTags::Get().GameplayEvent_Weapon_OutOfAmmo;
			Payload.Target = GetOwner();
			
			ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
	}
}

void UOWCombatComponent::HandleUltChanged(const FOnAttributeChangeData& Data)
{
	float MaxValue = BaseAtt ? BaseAtt->GetMaxUltimateCharge() : 0.f;
	OnAttributeChanged.Broadcast(FOWGameplayTags::Get().UI_Ult, Data.OldValue, Data.NewValue, MaxValue);
}

void UOWCombatComponent::HandleBarrierChanged(const FOnAttributeChangeData& Data)
{
	float MaxValue = SkillAtt ? SkillAtt->GetMaxBarrier() : 0.f;
	OnAttributeChanged.Broadcast(FOWGameplayTags::Get().UI_Barrier, Data.OldValue, Data.NewValue, MaxValue);
}

void UOWCombatComponent::HandleStaminaChanged(const FOnAttributeChangeData& Data)
{
	// 맵을 순회하며 변화한 스태미너가 어떤 Tag의 스태미너인지 탐색
	for (auto& Elem : SlotTagAttributeMap)
	{
		if (Elem.Value == Data.Attribute)
		{
			// 해당 SlotTag와 함께 변화량 방송
			float MaxStamina = SkillAtt ? SkillAtt->GetMaxStamina() : 0.0f;
			OnAttributeChanged.Broadcast(Elem.Key, Data.OldValue, Data.NewValue, MaxStamina);
		}
	}
		
	if (!GetOwner()->HasAuthority()) return; 

	if (SkillAtt && Data.Attribute == SkillAtt->GetStaminaAttribute())
	{
		float MaxStamina = SkillAtt->GetMaxStamina();

		// 스태미너가 맥스보다 낮아지는 순간 리젠
		if (Data.NewValue < MaxStamina)
		{
			FGameplayTag RegenStateTag = FOWGameplayTags::Get().State_Regen_Stamina;
            
			// 중복 발동 방지
			if (ASC && !ASC->HasMatchingGameplayTag(RegenStateTag))
			{
				//GA_RegenStamina 발동
				FGameplayTag EventTag = FOWGameplayTags::Get().GameplayEvent_Regen_Stamina;
				FGameplayEventData Payload;
				ASC->HandleGameplayEvent(EventTag, &Payload);
			}
		}
	}
}

void UOWCombatComponent::HandleCooldownTagChanged(const FGameplayTag CallbackTag, int32 NewCount, FGameplayTag SlotTag)
{
	float TimeRemaining = 0.0f;
	float TotalDuration = 0.0f;

	if (NewCount > 0)
	{
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CallbackTag));
		TArray<float> Durations = ASC->GetActiveEffectsTimeRemaining(Query);
		TArray<float> StartTimes = ASC->GetActiveEffectsDuration(Query);
		if (Durations.Num() > 0)
		{
			TimeRemaining = Durations[0];
			TotalDuration = StartTimes[0];
		}
	}
	OnCooldownChanged.Broadcast(SlotTag, TimeRemaining, TotalDuration);
}

void UOWCombatComponent::HandleRegenTagChanged(const FGameplayTag CallbackTag, int32 NewCount, FGameplayTag SlotTag,
	FGameplayAttribute RegenPeriodAttr)
{
	float TimeRemaining = 0.0f;
	float TotalDuration = 0.0f;

	if (NewCount > 0)
	{
		TimeRemaining = ASC->GetNumericAttribute(RegenPeriodAttr); // 리젠에 걸리는 시간
		TotalDuration = TimeRemaining;
	}
	OnCooldownChanged.Broadcast(SlotTag, TimeRemaining, TotalDuration);
}

