// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

#include "RakNetTypes.h"
#include "RakPeerInterface.h"
#include "GetTime.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "Gets.h"
#include "string.h"

#include "MyGameInstance.h"
#include "ReplicationActor.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/StringConv.h"
#include "GameFramework/Actor.h"
#include "MyLobby.generated.h"

UCLASS()
class RN4UE4_SAMPLE_API AMyLobby : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyLobby();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		int numOfPlayers = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int maxNumOfPlayers = 2;

	UPROPERTY()
		int playerNumber;

	UFUNCTION(BlueprintCallable, Category = "RakNet")
		void StartLobby();
};
