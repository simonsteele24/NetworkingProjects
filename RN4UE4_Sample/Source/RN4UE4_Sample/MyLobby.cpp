// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "ReplicationActor.h"
#include "MyLobby.h"

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
	ID_JUMP_INPUT = ID_USER_PACKET_ENUM + 11
};

// Sets default values
AMyLobby::AMyLobby()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyLobby::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyLobby::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	if (bCanRecieve)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
				case ID_NEW_INCOMING_CONNECTION:
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("INCOMNNING"));
					RakNet::BitStream bsOut;

					numOfPlayers++;

					bsOut.Write((RakNet::MessageID)ID_GIVE_PLAYER_NUMBER);
					bsOut.Write(numOfPlayers);

					break;
				}
				case ID_REPLICATION_MESSAGE:
				{

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

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
				case ID_GIVE_PLAYER_NUMBER:
				{
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(playerNumber);

			
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
				}
		
			}
		}
	}
}

