// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Client.h"

enum GameMessages
{
	ID_TEST_MESSAGE = ID_USER_PACKET_ENUM + 1
};

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
					address = packet->systemAddress;
				}
				case ID_START_GAME:
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("This text is filler, game should be starting thanks to server starting it"));
					address = packet->systemAddress;
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

	peer->Connect("172.16.4.129:", 60000, 0, 0);

	bCanRecieve = true;
}

// This function sends a test packet to the server
void AClient::SendTestMessage() 
{
	char str[512] = "";
	packet = peer->Receive();
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_TEST_MESSAGE);

	printf("Key Pressed!");
	std::cin >> str;
	bsOut.Write(str);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}

// This function disconnects from the server
void AClient::DisconnectFromServer() 
{
	RakNet::RakPeerInterface::DestroyInstance(peer);
}