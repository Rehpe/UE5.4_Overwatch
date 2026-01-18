#include "Character/OWHeroVoiceData.h"
#include "GAS/Tags/OWGameplayTags.h"

#if WITH_EDITOR
void UOWHeroVoiceData::FillRequiredTags()
{
	// 1. 추가할 태그 정의
	const FOWGameplayTags& Tags = FOWGameplayTags::Get();

	TArray<FGameplayTag> RequiredTags;
	RequiredTags.Add(Tags.Sound_Voice_Death);
	RequiredTags.Add(Tags.Sound_Voice_Pain);
	RequiredTags.Add(Tags.Sound_Voice_Jumping);
	RequiredTags.Add(Tags.Sound_Voice_JumpEnd);
	RequiredTags.Add(Tags.Sound_Voice_MeleeAttack);
	RequiredTags.Add(Tags.Sound_Voice_HealPack);
	RequiredTags.Add(Tags.Sound_Voice_HealthRestoredFriendly);
	RequiredTags.Add(Tags.Sound_Voice_Respawn);

	// 2. Map을 순회하며 없으면 추가
	bool bChanged = false;
	for (const FGameplayTag& Tag : RequiredTags)
	{
		if (!VoiceMap.Contains(Tag))
		{
			VoiceMap.Add(Tag, nullptr); // 빈 슬롯 생성
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