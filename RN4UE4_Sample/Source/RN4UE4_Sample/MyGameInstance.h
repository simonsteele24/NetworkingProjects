// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RN4UE4_SAMPLE_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite,Category = "Info")
	FString ipAddressTOConnectToo;
	
	
};
