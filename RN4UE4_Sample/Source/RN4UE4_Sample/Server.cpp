// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include "Server.h"


// Sets default values
AServer::AServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AServer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AServer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// This function starts up the server
void AServer::StartupServer() 
{
	peer = RakNet::RakPeerInterface::GetInstance();
	packet = NULL;
	
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Starting server!"));
}


// This function shuts down the server
void AServer::ShutdownServer() 
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Shutting down server!"));
	RakNet::RakPeerInterface::DestroyInstance(peer);
}