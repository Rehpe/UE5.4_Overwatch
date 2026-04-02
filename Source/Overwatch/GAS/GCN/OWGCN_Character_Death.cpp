// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GCN/OWGCN_Character_Death.h"

#include "Character/OWCharacterBase.h"
#include "Character/OWHeroData.h"
#include "Character/OWHeroVoiceData.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Tags/OWGameplayTags.h"
#include "Kismet/GameplayStatics.h"

UOWGCN_Character_Death::UOWGCN_Character_Death()
{
	GameplayCueTag = FOWGameplayTags::Get().GameplayCue_Character_Death;
}

bool UOWGCN_Character_Death::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
    AOWCharacterBase* OWCharacter = Cast<AOWCharacterBase>(MyTarget);
    if (!OWCharacter) return false;

    // DeathVoice 실행
   if (UOWHeroData* HeroData = OWCharacter->GetHeroData())
   {
       if (UOWHeroVoiceData* HVC = HeroData->VoiceData.Get())
       {
           FGameplayTag DeathVoiceTag = FOWGameplayTags::Get().Sound_Voice_Death;
           const TObjectPtr<USoundBase>* HeroDeathVoicePtr = HVC->VoiceMap.Find(DeathVoiceTag);
           if (HeroDeathVoicePtr && *HeroDeathVoicePtr)
           {
               TObjectPtr<USoundBase> HeroDeathVoice = *HeroDeathVoicePtr;
               UGameplayStatics::PlaySoundAtLocation(OWCharacter, HeroDeathVoice, OWCharacter->GetActorLocation());
           }
       }
   }
    
   // 피격 방향으로 Impulse
    if (USkeletalMeshComponent* Mesh = OWCharacter->GetMesh())
    {
        // Impulse
        FVector ImpulseDirection = FVector::ZeroVector;
        
        if (Parameters.EffectContext.IsValid())
        {
            // HitResult 있을 때
            if (const FHitResult* Hit = Parameters.EffectContext.GetHitResult())
            {
                // TraceStart(총구) -> TraceEnd(맞은 곳) 방향
                ImpulseDirection = (Hit->TraceEnd - Hit->TraceStart).GetSafeNormal();
            }
            // HitResult는 없지만 EffectCauser가 있는 경우 (광역뎀)
            else if (AActor* CauserActor = Parameters.EffectContext.GetEffectCauser())
            {
                FVector CauserLocation = CauserActor->GetActorLocation();
                FVector MyLocation = OWCharacter->GetActorLocation();
                // 폭발 중심 -> 내 위치 방향
                ImpulseDirection = (MyLocation - CauserLocation).GetSafeNormal();
            }
        }

        if (!ImpulseDirection.IsZero())
        {
            // Impulse Debug
            DrawDebugDirectionalArrow(
                GetWorld(), 
                OWCharacter->GetActorLocation(), 
                OWCharacter->GetActorLocation() + (ImpulseDirection * 200.0f), 
                50.0f, FColor::Red, false, 3.0f, 0, 5.0f
            );
            
            float ImpulsePower = 5000.0f;
            
            // 척추가 있다면 척추를 민다
            FName BoneName = FName("bone_0002"); 
            if (Mesh->GetBoneIndex(BoneName) != INDEX_NONE)
            {
                Mesh->AddImpulse(ImpulseDirection * ImpulsePower, BoneName, true); 
            }
            else
            {
                Mesh->AddImpulse(ImpulseDirection * ImpulsePower, NAME_None, true); 
            }
        }
    }
    return Super::OnExecute_Implementation(MyTarget, Parameters);
}
