﻿// Fill out your copyright notice in the Description page of Project Settings.



#include "RN4UE4_Sample.h"

#include "IRakNet.h"



#include "Ping.h"



DEFINE_LOG_CATEGORY(RN4UE4Sample);


// Sets default values
APing::APing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APing::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APing::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (waitReceivedData == false)
	{
		return;
	}


	p = client->Receive();
	if (p == 0)
	{
		UE_LOG(RN4UE4Sample, Log, TEXT("waiting data..."));
		return;
	}

	waitReceivedData = false;

	// Check if this is a network message packet
	switch (p->data[0])
	{
	case ID_UNCONNECTED_PONG:
	{
		unsigned int dataLength;
		RakNet::TimeMS time;
		RakNet::BitStream bsIn(p->data, p->length, false);
		bsIn.IgnoreBytes(1);
		bsIn.Read(time);
		dataLength = p->length - sizeof(unsigned char) - sizeof(RakNet::TimeMS);

		FString sysAddress = FString(p->systemAddress.ToString(true));
		UE_LOG(RN4UE4Sample, Log, TEXT("ID_UNCONNECTED_PONG from SystemAddress %s."), *sysAddress);
		UE_LOG(RN4UE4Sample, Log, TEXT("Time is %i"), time);
		UE_LOG(RN4UE4Sample, Log, TEXT("Data is %i bytes long"), dataLength);


		if (dataLength > 0)
		{
			char* charData = (char*)(p->data + sizeof(unsigned char) + sizeof(RakNet::TimeMS));
			FString strData = FString(UTF8_TO_TCHAR(charData));
			UE_LOG(RN4UE4Sample, Log, TEXT("String is %s, length is %d "), *strData, strlen(charData));

		}

		// In this sample since the client is not running a game we can save CPU cycles by
		// Stopping the network threads after receiving the pong.
		//client->Shutdown(100);
	}
	break;
	case ID_UNCONNECTED_PING:
		break;
	case ID_UNCONNECTED_PING_OPEN_CONNECTIONS:
		break;
	}

	client->DeallocatePacket(p);

	RakNet::RakPeerInterface::DestroyInstance(server);
	RakNet::RakPeerInterface::DestroyInstance(client);
	UE_LOG(RN4UE4Sample, Log, TEXT("client finished."));

}

void APing::StartServer(FString responseString)
{
	server = RakNet::RakPeerInterface::GetInstance();
	int i = server->GetNumberOfAddresses();

	char* charString = TCHAR_TO_UTF8(*responseString);
	//char* charString = TCHAR_TO_UTF8((L""));
	UE_LOG(RN4UE4Sample, Log, TEXT("Server response string length: %d"), strlen(charString));
	UE_LOG(RN4UE4Sample, Log, TEXT("Server response string: %s"), *FString(UTF8_TO_TCHAR(charString)));
	//char* charString = TCHAR_TO_ANSI(L"abc123");
	//char enumData[512] = TCHAR_TO_ANSI(L"abc123中文字串あいうえお");
	//char enumData[512] = "abc123";
	server->SetOfflinePingResponse(charString, (const unsigned int)strlen(charString) + 1);

	// The server has to be started to respond to pings.
	RakNet::SocketDescriptor socketDescriptor(60000, 0);
	bool b = server->Startup(2, &socketDescriptor, 1) == RakNet::RAKNET_STARTED;
	server->SetMaximumIncomingConnections(2);
	if (b)
	{
		UE_LOG(RN4UE4Sample, Log, TEXT("Server started, waiting for connections."));
	}
	else
	{
		UE_LOG(RN4UE4Sample, Log, TEXT("Server start failed!"));
	}
}

void APing::ClientPing()
{
	UE_LOG(RN4UE4Sample, Log, TEXT("OnRakNetPingClient"));

	client = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor socketDescriptor(0, 0);
	client->Startup(1, &socketDescriptor, 1);
	client->Ping("127.0.0.1", 60000, false);

	waitReceivedData = true;
	
}
