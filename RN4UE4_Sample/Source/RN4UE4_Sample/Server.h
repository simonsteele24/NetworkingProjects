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
	virtual void Tick(float DeltaSeconds) override;

public:	

	// Purpose : Called to start the server
	// In : None
	// Out : None
	UFUNCTION(BlueprintCallable, Category = "RakNet") void StartupServer();

	// Purpose : Called to shutdown the actual server
	// In : None
	// Out : None
	UFUNCTION(BlueprintCallable, Category = "RakNet") void ShutdownServer();

	// Purpose : Called if a player has crossed the finish line to declare a winner
	// In : playerNum(The player number who won)
	// Out : None
	UFUNCTION(BlueprintCallable, Category = "Raknet") void DeclareWinner(int playerNum);

	// Purpose : Called to start the actual game
	// In : None
	// Out : None
	UFUNCTION(BlueprintCallable, Category = "RakNet") void StartGame();

	// Purpose : To cycle through all of the custom messages to get the correct message
	// In : None
	// Out : None
	UFUNCTION(BlueprintCallable, Category = "RakNet") void GoThroughRecievedPackets();

	/* Public Vars */
	UPROPERTY(EditAnywhere) float cubeSpeed; // Represents the speed the cube can actually go

private:

	/* Raknet Vals */
	RakNet::RakPeerInterface* peer; // Represents the interface with raknet
	RakNet::Packet* packet; // Represents the current packet being sent or recieved
	TArray<RakNet::SystemAddress> clients; // Represents all current clients on the server

	/* Private Vars */
	UPROPERTY() int numOfPlayers = 0; // Represents the number of players currently on the server
	UPROPERTY() bool bCanRecieve = false; // Represents if the server can currently recieve packets
	UPROPERTY() FVector repLocation = FVector(0.0f, 0.0f, 0.0f); // Represents the replicated actor's location
};
