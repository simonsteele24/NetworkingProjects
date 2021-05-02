// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
#include "MyLobby.h"
#include "Client.generated.h"
UCLASS()
class RN4UE4_SAMPLE_API AClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClient();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "Raknet")
	void ConnectToServer();
	
	UFUNCTION(BlueprintCallable, Category = "Raknet")
	void SendTestMessage();

	UFUNCTION(BlueprintCallable, Category = "Raknet")
	void Jump();

	UFUNCTION(BlueprintCallable, Category = "Raknet")
	void DisconnectFromServer();

	UFUNCTION(BlueprintCallable, Category = "Raknet")
	void MoveForwardServer(float input);

	UFUNCTION(BlueprintCallable, Category = "Raknet")
	void MoveRightServer(float input);

	UPROPERTY(EditAnywhere)
	TSubclassOf<AReplicationActor> replication;

	UPROPERTY()
	int playerNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString newAddress;

	UPROPERTY(EditAnywhere)
	int numOfPlayers = 0;

	UPROPERTY(EditAnywhere)
	int maxNumOfPlayers = 2;

	AMyLobby* lobbyActor;

private:
	RakNet::RakPeerInterface* peer;
	RakNet::Packet* packet;
	RakNet::SystemAddress address;
	UMyGameInstance* gameInstance;

	UPROPERTY()
	bool bCanRecieve = false;

	UPROPERTY()
	AActor * repActor;
};
