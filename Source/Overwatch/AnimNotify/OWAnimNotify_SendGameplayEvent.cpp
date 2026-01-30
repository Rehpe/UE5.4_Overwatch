// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/OWAnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UOWAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		FGameplayEventData Payload;
		
		// GAS 라이브러리를 통해 안전하게 이벤트 발송
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, Payload);
	}
}
