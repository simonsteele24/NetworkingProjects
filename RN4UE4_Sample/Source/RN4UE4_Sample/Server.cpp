// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include "Server.h"

enum GameMessages
{
	ID_TEST_MESSAGE = ID_USER_PACKET_ENUM + 1
};

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

	if (bCanRecieve) 
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_TEST_MESSAGE:
			{
				RakNet::RakString rs = RakNet::RakString();
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HI!"));
			}
			case ID_NEW_INCOMING_CONNECTION:
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("INCOMNNING"));
			}
			}
		}
	}

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

	bCanRecieve = true;
}


// This function shuts down the server
void AServer::ShutdownServer() 
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Shutting down server!"));
	RakNet::RakPeerInterface::DestroyInstance(peer);
}