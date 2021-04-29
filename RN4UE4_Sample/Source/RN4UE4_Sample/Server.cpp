// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include "Server.h"

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
					break;
				}
				case ID_NEW_INCOMING_CONNECTION:
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("INCOMNNING"));
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_REPLICATION_MESSAGE);
					bsOut.Write(replication);

					clients.Add(packet->systemAddress);

					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
					break;
				}
				case ID_INPUT_MOVE_FORWARD_MESSAGE:
				{
					float input;

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(input);

					repLocation += input * FVector(1, 0, 0) * cubeSpeed * GetWorld()->GetDeltaSeconds();

					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_MOVE_FORWARD_MESSAGE);

					bsOut.Write(repLocation);

					for (int i = 0; i < clients.Num(); i++)
					{
						if (clients[i] != packet->systemAddress) 
						{
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
						}
					}

					break;
				}
				case ID_INPUT_MOVE_RIGHT_MESSAGE:
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Moving!"));

					float input;

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(input);

					repLocation += input * FVector(0, 1, 0) * cubeSpeed * GetWorld()->GetDeltaSeconds();

					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_MOVE_RIGHT_MESSAGE);

					bsOut.Write(repLocation);
					
					for (int i = 0; i < clients.Num(); i++)
					{
						if (clients[i] != packet->systemAddress)
						{
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
						}
					}

					break;
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
	if (peer != nullptr)
	{
		peer->Shutdown(300);
		RakNet::RakPeerInterface::DestroyInstance(peer);
	}
}

// This function shuts down the server
void AServer::StartGame()
{
	peer = RakNet::RakPeerInterface::GetInstance();
	packet = NULL;
	char str[512] = "";

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_START_GAME);

	bsOut.Write(str);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
}