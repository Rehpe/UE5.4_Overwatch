// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/OWDamageExecution.h"

#include "OWAttributeSet_Base.h"
#include "GAS/Tags/OWGameplayTags.h"

UOWDamageExecution::UOWDamageExecution()
{
}

void UOWDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	// 들어온 데미지
	float BaseDamage = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(FOWGameplayTags::Get().Data_Damage, false, 0.0f);
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float FinalDamage = BaseDamage;

	// ========= Damage Calc =========

	// AssetTag, Dynamic Tag(런타임태그) 모두 검사
	FGameplayTagContainer AllAssetTags;
	Spec.GetAllAssetTags(AllAssetTags);
	
	// HeadShot 판정
	if (Spec.GetDynamicAssetTags().HasTag(FOWGameplayTags::Get().Damage_Modifier_Headshot))
	{
		FinalDamage *= 3.0f;
	}

	// ======== Final Damage ===========
	if (FinalDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				UOWAttributeSet_Base::GetDamageAttribute(), 
				EGameplayModOp::Additive, 
				FinalDamage
			)
		);
	}
}
