// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/OWAssetManager.h"

#include "GAS/Tags/OWGameplayTags.h"

UOWAssetManager& UOWAssetManager::Get()
{
	check(GEngine);
	// 현재 활성화된 에셋 매니저를 가져옴
	UOWAssetManager* OWAssetManager = Cast<UOWAssetManager>(GEngine->AssetManager);
	return *OWAssetManager;
}

void UOWAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	FOWGameplayTags::InitializeNativeTags();
    
	UE_LOG(LogTemp, Warning, TEXT("[OWAssetManager] Native Tags Loaded via AssetManager!"));
}
