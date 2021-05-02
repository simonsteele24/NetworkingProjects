// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ReplicationActor.generated.h"

UCLASS()
class RN4UE4_SAMPLE_API AReplicationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReplicationActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int playerNum = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOwner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldJump = false;
};
