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
	ID_MOVE_RIGHT_MESSAGE = ID_USER_PACKET_ENUM + 6,
	ID_ADD_PLAYER = ID_USER_PACKET_ENUM + 7,
	ID_GIVE_NEW_PLAYER_INFO = ID_USER_PACKET_ENUM + 8,
	ID_GIVE_PLAYER_NUMBER = ID_USER_PACKET_ENUM + 9
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

					numOfPlayers++;


					bsOut.Write((RakNet::MessageID)ID_GIVE_PLAYER_NUMBER);
					bsOut.Write(numOfPlayers++);

					for (int i = 0; i < clients.Num(); i++)
					{
						RakNet::BitStream bsOutTwo;
						bsOutTwo.Write((RakNet::MessageID)ID_ADD_PLAYER);
						bsOutTwo.Write(numOfPlayers);
						peer->Send(&bsOutTwo, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
					}

					TArray<AActor*> out;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReplicationActor::StaticClass(), out);

					for (int i = 0; i < out.Num(); i++)
					{
						AReplicationActor * actor = Cast<AReplicationActor>(out[i]);
						RakNet::BitStream bsOutThree;
						bsOutThree.Write((RakNet::MessageID)ID_GIVE_NEW_PLAYER_INFO);
						bsOutThree.Write(out[i]->GetActorLocation());
						bsOutThree.Write(actor->playerNum);
						peer->Send(&bsOutThree, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
					}

					clients.Add(packet->systemAddress);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
					break;
				}
				case ID_INPUT_MOVE_FORWARD_MESSAGE:
				{
					int num = 0;

					FVector location(0.0f, 0.0f, 0.0f);
					float input;

					TArray<AActor*> out;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReplicationActor::StaticClass(), out);

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(input);
					bsIn.Read(num);

					for (int i = 0; i < out.Num(); i++)
					{
						AReplicationActor * actor = Cast<AReplicationActor>(out[i]);
						if (actor->playerNum == num)
						{
							actor->AddActorWorldOffset(input * FVector(1, 0, 0) * cubeSpeed * GetWorld()->GetDeltaSeconds());
							location = actor->GetActorLocation();
						}
					}

					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_MOVE_FORWARD_MESSAGE);

					bsOut.Write(repLocation);
					bsOut.Write(num);

					for (int i = 0; i < clients.Num(); i++)
					{
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
					}

					break;
				}
				case ID_INPUT_MOVE_RIGHT_MESSAGE:
				{
					int num = 0;

					FVector location(0.0f, 0.0f, 0.0f);
					float input;

					TArray<AActor*> out;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReplicationActor::StaticClass(), out);

					RakNet::RakString rs = RakNet::RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(input);
					bsIn.Read(num);

					for (int i = 0; i < out.Num(); i++) 
					{
						AReplicationActor * actor = Cast<AReplicationActor>(out[i]);
						if (actor->playerNum == num) 
						{
							actor->AddActorWorldOffset(input * FVector(0, 1, 0) * cubeSpeed * GetWorld()->GetDeltaSeconds());
							location = actor->GetActorLocation();
						}
					}

					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_MOVE_RIGHT_MESSAGE);

					bsOut.Write(repLocation);
					bsOut.Write(num);
					
					for (int i = 0; i < clients.Num(); i++)
					{
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
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