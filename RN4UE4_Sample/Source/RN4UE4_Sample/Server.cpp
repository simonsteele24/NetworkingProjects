// Fill out your copyright notice in the Description page of Project Settings.

#include "RN4UE4_Sample.h"
#include "StaticReplicationActor.h"
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
	ID_GIVE_PLAYER_NUMBER = ID_USER_PACKET_ENUM + 9,
	ID_UPDATE_LOCATION = ID_USER_PACKET_ENUM + 10,
	ID_JUMP_INPUT = ID_USER_PACKET_ENUM + 11,
	ID_GET_NUMBER_PLAYERS = ID_USER_PACKET_ENUM + 12,
	ID_UPDATE_TRANSFORM = ID_USER_PACKET_ENUM + 13,
	ID_DECLARE_WINNER = ID_USER_PACKET_ENUM + 14
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

	TArray<AActor*> out;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticReplicationActor::StaticClass(), out);

	for (int i = 0; i < out.Num(); i++)
	{
		AStaticReplicationActor * actor = Cast<AStaticReplicationActor>(out[i]);

		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_UPDATE_TRANSFORM);

		bsOut.Write(actor->GetActorTransform());
		bsOut.Write(actor->actorNumber);

		for (int i = 1; i < clients.Num(); i++)
		{
			peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
		}
	}

	TArray<AActor*> in;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReplicationActor::StaticClass(), in);

	for (int i = 0; i < in.Num(); i++) 
	{
		AReplicationActor * actor = Cast<AReplicationActor>(in[i]);

		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_UPDATE_LOCATION);

		bsOut.Write(actor->GetActorLocation());
		bsOut.Write(actor->playerNum);

		for (int i = 1; i < clients.Num(); i++)
		{
			peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
		}
	}

	if (bCanRecieve) 
	{
		GoThroughRecievedPackets();
	}

}


// Purpose : Called to start the server
// In : None
// Out : None
void AServer::StartupServer() 
{
	// Create peer and reset packet
	peer = RakNet::RakPeerInterface::GetInstance();
	packet = NULL;
	
	// Startup the actual server
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);

	// Set max clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);
	
	// Print for proof
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Starting server!"));

	// Set server to recieve
	bCanRecieve = true;
}


// Purpose : Called to shutdown the actual server
// In : None
// Out : None
void AServer::ShutdownServer() 
{
	// Print for proof
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Shutting down server!"));
	
	// Make sure interface is valid
	if (peer != nullptr)
	{
		// Shutdown and destroy interface
		peer->Shutdown(300);
		RakNet::RakPeerInterface::DestroyInstance(peer);
	}
}


// Purpose : Called to start the actual game
// In : None
// Out : None
void AServer::StartGame()
{
	// Startup peer and set packet to null
	peer = RakNet::RakPeerInterface::GetInstance();
	packet = NULL;
	char str[512] = "";

	//Create a bitstream for packet data
	RakNet::BitStream bsOut;

	// Write message
	bsOut.Write((RakNet::MessageID)ID_START_GAME);

	// Write the string with message
	bsOut.Write(str);

	// Send to recieving client
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
}


// Purpose : Called if a player has crossed the finish line to declare a winner
// In : playerNum(The player number who won)
// Out : None
void AServer::DeclareWinner(int playerNum) 
{
	// Cycle thru all clients
	for (int i = 0; i < clients.Num(); i++) 
	{
		// Get current packet
		packet = peer->Receive();

		// Create bitstream for packet dta
		RakNet::BitStream bsOut;

		// Write message identifier
		bsOut.Write((RakNet::MessageID)ID_DECLARE_WINNER);
		
		// write a boolean to give the client the chance to see if the winner is them
		bsOut.Write(i + 1 == playerNum);

		// Send packet to given client
		peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
	}
}


// Purpose : To cycle through all of the custom messages to get the correct message
// In : None
// Out : None
void AServer::GoThroughRecievedPackets()
{
	// Cycle thru recieved packets
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		// switch on packet data
		switch (packet->data[0])
		{

		// Run for test message
		case ID_TEST_MESSAGE:
		{
			RakNet::RakString rs = RakNet::RakString();
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rs);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HI!"));
			break;
		}

		// Run for incoming connection
		case ID_NEW_INCOMING_CONNECTION:
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("INCOMNNING"));
			RakNet::BitStream bsOut;

			numOfPlayers++;

			bsOut.Write((RakNet::MessageID)ID_GIVE_PLAYER_NUMBER);
			bsOut.Write(numOfPlayers);

			//yo simon this part dont even run
			for (int i = 0; i < clients.Num(); i++)
			{
				RakNet::BitStream bsOutTwo;
				bsOutTwo.Write((RakNet::MessageID)ID_ADD_PLAYER);
				bsOutTwo.Write(numOfPlayers);
				peer->Send(&bsOutTwo, HIGH_PRIORITY, RELIABLE_ORDERED, 1, clients[i], false);
			}

			TArray<AActor*> out;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReplicationActor::StaticClass(), out);

			//yo simon this part dont even run
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

		// Run for getting player numbers
		case ID_GET_NUMBER_PLAYERS:
		{

			RakNet::BitStream bsOut;

			bsOut.Write((RakNet::MessageID)ID_GET_NUMBER_PLAYERS);
			bsOut.Write(numOfPlayers);
			bsOut.Write(MAX_CLIENTS);
			peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			break;
		}

		// Run for moving forward
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
					actor->SetActorLocation(actor->GetActorLocation() + (input * FVector(1, 0, 0) * cubeSpeed * GetWorld()->GetDeltaSeconds()));
					location = actor->GetActorLocation();
				}
			}
			break;
		}

		// Run for moving right
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
					actor->SetActorLocation(actor->GetActorLocation() + (input * FVector(0, 1, 0) * cubeSpeed * GetWorld()->GetDeltaSeconds()));
					location = actor->GetActorLocation();
				}
			}
			break;
		}

		// Run for jumping
		case ID_JUMP_INPUT:
		{
			int num = 0;

			TArray<AActor*> out;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReplicationActor::StaticClass(), out);

			RakNet::RakString rs = RakNet::RakString();
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(num);

			for (int i = 0; i < out.Num(); i++)
			{
				AReplicationActor * actor = Cast<AReplicationActor>(out[i]);
				if (actor->playerNum == num)
				{
					actor->bShouldJump = true;
				}
			}
			break;
		}
		}
	}
}