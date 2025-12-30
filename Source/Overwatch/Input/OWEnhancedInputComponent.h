// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "OWInputConfig.h"
#include "OWEnhancedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class OVERWATCH_API UOWEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// [template] Lyra Base로 어떤 클래스 함수라도 연결 가능
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const class UOWInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
	{
		check(InputConfig);

		// Config에 정의된 [GAS 입력 액션] 배열을 순회하며 바인딩
		for (const FOWTaggedInputAction& Action : InputConfig->GASInputActions)
		{
			if (Action.InputAction && Action.InputTag.IsValid())
			{
				if (PressedFunc)	// 눌렀을 때
				{
					// 바인딩할 액션, TriggerEvent, 클래스, 발동할 함수, 태그
					BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag).GetHandle());
				}
				if (ReleasedFunc)	// 뗐을 때
				{
					// 바인딩할 액션, TriggerEvent, 클래스, 발동할 함수, 태그
					BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
				}
			}
		}
	}
};
