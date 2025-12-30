// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// OwLog Category 추가
DECLARE_LOG_CATEGORY_EXTERN(OwLog, Log, All);

// 1. 호출된 함수의 이름과 라인 매크로를 String화시킴
#define LOG_CALLINFO (FString::Printf(TEXT("%s(%d)"), *FString(__FUNCTION__), __LINE__))

// 2. Verbosity를 제외하고, 텍스트(TEXT("하고싶은말"))로 로그를 찍는 매크로
#define OWLOG_WARNING(Format, ...) UE_LOG(OwLog, Warning, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define OWLOG_ERROR(Format, ...) UE_LOG(OwLog, Error, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

//4. 게임 스크린에 간단히 찍는 매크로
#define OWLOG_SCREEN(Format, ...) if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__));}
