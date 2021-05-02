// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define MAX_CLIENTS 2
#define SERVER_PORT 60000

#include "RakNetTypes.h"
#include "RakPeerInterface.h"
#include "GetTime.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "Gets.h"

#include "Kismet/GameplayStatics.h"
#include "ReplicationActor.h"
#include "GameFramework/Actor.h"
#include "Server.generated.h"

UCLASS()
class RN4UE4_SAMPLE_API AServer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AServer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to start server
	UFUNCTION(BlueprintCallable, Category = "RakNet") 
	void StartupServer();

	// Called to shut down server
	UFUNCTION(BlueprintCallable, Category = "RakNet")
	void ShutdownServer();

	UFUNCTION(BlueprintCallable, Category = "Raknet")
	void DeclareWinner(int playerNum);

	// Called to shut down server
	UFUNCTION(BlueprintCallable, Category = "RakNet")
	void StartGame();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AReplicationActor> replication;

	UPROPERTY(EditAnywhere)
	float cubeSpeed;

private:
	RakNet::RakPeerInterface* peer;
	RakNet::Packet* packet;

	TArray<RakNet::SystemAddress> clients;

	UPROPERTY()
	int numOfPlayers = 0;

	UPROPERTY()
	bool bCanRecieve = false;

	UPROPERTY()
	FVector repLocation = FVector(0.0f, 0.0f, 0.0f);
};
