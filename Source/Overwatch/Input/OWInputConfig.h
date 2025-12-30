#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "OWInputConfig.generated.h"

// 오버워치의 모든 입력 액션을 정의하는 데이터 에셋

USTRUCT(BlueprintType)
struct FOWTaggedInputAction
{
    GENERATED_BODY()

    // 입력 액션(IA) + 태그 구조체
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<class UInputAction> InputAction = nullptr;     

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag InputTag;
};

UCLASS()
class OVERWATCH_API UOWInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    // 기본 동작
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> Move;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> Look;

    // GAS 기능 동작 배열
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TArray<FOWTaggedInputAction> GASInputActions;
};