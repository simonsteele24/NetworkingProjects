// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ReplicationActor.h"
#include "Client.h"

enum GameMessages
{
	ID_TEST_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_REPLICATION_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_INPUT_MOVE_FORWARD_MESSAGE = ID_USER_PACKET_ENUM + 3,
	ID_MOVE_FORWARD_MESSAGE = ID_USER_PACKET_ENUM + 4,
	ID_INPUT_MOVE_RIGHT_MESSAGE = ID_USER_PACKET_ENUM + 5,
	ID_MOVE_RIGHT_MESSAGE = ID_USER_PACKET_ENUM + 6
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
					break;
				}
				case ID_START_GAME:
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("This text is filler, game should be starting thanks to server starting it"));
					address = packet->systemAddress;
					break;
				}
				case ID_REPLICATION_MESSAGE:
				{
					TSubclassOf<AReplicationActor> replicationClass;

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(replicationClass);

					if (replicationClass != nullptr) 
					{
						FVector Location(0.0f, 0.0f, 0.0f);
						FRotator Rotation(0.0f, 0.0f, 0.0f);
						FActorSpawnParameters SpawnInfo;

						repActor = GetWorld()->SpawnActor<AReplicationActor>(replicationClass, Location, Rotation, SpawnInfo);
					}

					break;
				}
				case ID_MOVE_FORWARD_MESSAGE:
				{
					FVector location(0.0f, 0.0f, 0.0f);
					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(location);

					repActor->SetActorLocation(location);

					break;
				}
				case ID_MOVE_RIGHT_MESSAGE:
				{
					FVector location(0.0f, 0.0f, 0.0f);
					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(location);

					repActor->SetActorLocation(location);

					break;
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

	peer->Connect("184.171.152.82:", 60000, 0, 0);

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

//
void AClient::MoveForwardServer(float input) 
{
	packet = peer->Receive();
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_INPUT_MOVE_FORWARD_MESSAGE);
	bsOut.Write(input);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}

//
void AClient::MoveRightServer(float input)
{
	packet = peer->Receive();
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_INPUT_MOVE_RIGHT_MESSAGE);
	bsOut.Write(input);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}