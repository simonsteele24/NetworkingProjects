// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RakNetTypes.h"
#include "RakPeerInterface.h"
#include "GetTime.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "Gets.h"

#include "GameFramework/Actor.h"
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
	
private:
	RakNet::RakPeerInterface* peer;
	RakNet::Packet* packet;
	RakNet::SystemAddress address;

	UPROPERTY()
	bool bCanRecieve = false;
};
