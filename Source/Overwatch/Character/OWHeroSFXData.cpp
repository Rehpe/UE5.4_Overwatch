// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OWHeroSFXData.h"
#include "GAS/Tags/OWGameplayTags.h"

#if WITH_EDITOR

void UOWHeroSFXData::FillRequiredTags()
{
	// 1. 추가할 태그 정의
	const FOWGameplayTags& Tags = FOWGameplayTags::Get();

	TArray<FGameplayTag> RequiredTags;
	RequiredTags.Add(Tags.Sound_SFX_MeleeAttack);

	// 2. Map을 순회하며 없으면 추가
	bool bChanged = false;
	for (const FGameplayTag& Tag : RequiredTags)
	{
		if (!SFXMap.Contains(Tag))
		{
			SFXMap.Add(Tag, nullptr); // 빈 슬롯 생성
			bChanged = true;
		}
	}
	// 3. 변경사항이 있으면 에셋을 '수정됨(Dirty)' 상태로 표시
	if (bChanged)
	{
		MarkPackageDirty();
	}
}
#endif
