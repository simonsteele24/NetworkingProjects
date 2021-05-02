// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "ReplicationActor.h"
#include "StaticReplicationActor.h"
#include "Client.h"

enum GameMessages
{
	ID_TEST_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_REPLICATION_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_INPUT_MOVE_FORWARD_MESSAGE = ID_USER_PACKET_ENUM + 3,
	ID_MOVE_FORWARD_MESSAGE = ID_USER_PACKET_ENUM + 4,
	ID_INPUT_MOVE_RIGHT_MESSAGE = ID_USER_PACKET_ENUM + 5,
	ID_MOVE_RIGHT_MESSAGE = ID_USER_PACKET_ENUM + 6,
	ID_ADD_PLAYER = ID_USER_PACKET_ENUM + 7,
	ID_GIVE_NEW_PLAYER_INFO = ID_USER_PACKET_ENUM + 8,
	ID_GIVE_PLAYER_NUMBER = ID_USER_PACKET_ENUM + 9,
	ID_UPDATE_LOCATION = ID_USER_PACKET_ENUM + 10,
	ID_JUMP_INPUT = ID_USER_PACKET_ENUM + 11,
	ID_GET_NUMBER_PLAYERS = ID_USER_PACKET_ENUM + 12,
	ID_UPDATE_TRANSFORM = ID_USER_PACKET_ENUM + 13
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
	gameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	//TArray<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyLobby::StaticClass(), FoundActors);
	//lobbyActor = Cast<AMyLobby>(FoundActors[0]);
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
					RakNet::BitStream bsOut;

					//bsOut.Write((RakNet::MessageID)ID_GET_NUMBER_PLAYERS);
					//peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
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

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

					FVector Location(0.0f, 0.0f, 0.0f);
					FRotator Rotation(0.0f, 0.0f, 0.0f);
					FActorSpawnParameters SpawnInfo;

					repActor = GetWorld()->SpawnActor<AReplicationActor>(replication, Location, Rotation, SpawnInfo);

					break;
				}
				case ID_GIVE_NEW_PLAYER_INFO:
				{
					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

					int num = 0;
					FVector Location(0.0f, 0.0f, 0.0f);
					FRotator Rotation(0.0f, 0.0f, 0.0f);
					FActorSpawnParameters SpawnInfo;

					bsIn.Read(Location);
					bsIn.Read(num);

					AReplicationActor * actor = GetWorld()->SpawnActor<AReplicationActor>(replication, Location, Rotation, SpawnInfo);
					actor->playerNum = num;
					actor->bIsOwner = playerNumber == 1;
					break;
				}
				case ID_GET_NUMBER_PLAYERS:
				{
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				

					int serverCurrentPlayerNum = 0;
					int serverMaxPlayerNum = 0;

					bsIn.Read(serverCurrentPlayerNum);
					bsIn.Read(serverMaxPlayerNum);
			
					numOfPlayers = serverCurrentPlayerNum;
					maxNumOfPlayers = serverMaxPlayerNum;
					lobbyActor->numOfPlayers = numOfPlayers;
					lobbyActor->maxNumOfPlayers = maxNumOfPlayers;
					break;
				}
				case ID_GIVE_PLAYER_NUMBER:
				{
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(playerNumber);

					FVector Location(100.0f * playerNumber, 0.0f, 100.0f);
					FRotator Rotation(0.0f, 0.0f, 0.0f);
					FActorSpawnParameters SpawnInfo;

					repActor = GetWorld()->SpawnActor<AReplicationActor>(replication, Location, Rotation, SpawnInfo);
					AReplicationActor * actor = Cast<AReplicationActor>(repActor);
					actor->playerNum = playerNumber;
					actor->bIsOwner = playerNumber == 1;
					//lobbyActor->playerNumber = playerNumber;
					break;
				}
				case ID_ADD_PLAYER:
				{
					int num = 0;

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(num);

					FVector Location(100.0f * num, 0.0f, 100.0f);
					FRotator Rotation(0.0f, 0.0f, 0.0f);
					FActorSpawnParameters SpawnInfo;

					AReplicationActor * newActor = GetWorld()->SpawnActor<AReplicationActor>(replication, Location, Rotation, SpawnInfo);
					newActor->playerNum = num;
					newActor->bIsOwner = num == 0;
					break;
				}
				case ID_UPDATE_TRANSFORM:
				{
					int num = 0;
					FTransform transform;

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(transform);
					bsIn.Read(num);

					TArray<AActor*> out;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticReplicationActor::StaticClass(), out);
					for (int i = 0; i < out.Num(); i++)
					{
						AStaticReplicationActor * actor = Cast<AStaticReplicationActor>(out[i]);
						if (actor->actorNumber == num)
						{
							actor->SetActorTransform(transform);
						}
					}

				}
				case ID_UPDATE_LOCATION:
				{
					FVector location(0.0f, 0.0f, 0.0f);
					int num;

					TArray<AActor*> out;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReplicationActor::StaticClass(), out);

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(location);
					bsIn.Read(num);

					for (int i = 0; i < out.Num(); i++)
					{
						AReplicationActor * actor = Cast<AReplicationActor>(out[i]);
						if (actor->playerNum == num)
						{
							actor->SetActorLocation(location);
						}
					}
					break;
				}
			}
		}
	}
}

// This function tries to connect to the server
void AClient::ConnectToServer() 
{
	if (playerNumber == 0) 
	{
		peer = RakNet::RakPeerInterface::GetInstance();
		packet = NULL;

		address = RakNet::SystemAddress();

		RakNet::SocketDescriptor sd = RakNet::SocketDescriptor();
		peer->Startup(1, &sd, 1);

		const char* result = StringCast<ANSICHAR>(*newAddress).Get();
		printf(result);

		peer->Connect(result, 60000, 0, 0);

		bCanRecieve = true;
	}
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
	if (peer != nullptr)
	{
		packet = peer->Receive();
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_INPUT_MOVE_FORWARD_MESSAGE);
		bsOut.Write(input);
		bsOut.Write(playerNumber);
		peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
	}

}

//
void AClient::MoveRightServer(float input)
{
	if (peer != nullptr)
	{
		packet = peer->Receive();
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_INPUT_MOVE_RIGHT_MESSAGE);
		bsOut.Write(input);
		bsOut.Write(playerNumber);
		peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
	}
}

void AClient::Jump() 
{
	if (peer != nullptr)
	{
		packet = peer->Receive();
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_JUMP_INPUT);
		bsOut.Write(playerNumber);
		peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
	}
}