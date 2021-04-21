// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include "Client.h"


// Sets default values
AClient::AClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AClient::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClient::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bCanRecieve) 
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
				case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Connected!"));
				}
			}
		}
	}
}

// This function tries to connect to the server
void AClient::ConnectToServer() 
{
	peer = RakNet::RakPeerInterface::GetInstance();
	packet = NULL;

	address = RakNet::SystemAddress();

	RakNet::SocketDescriptor sd = RakNet::SocketDescriptor();

	peer->Startup(1, &sd, 1);

	peer->Connect("184.171.152.82", 60000, 0, 0);

	bCanRecieve = true;
}