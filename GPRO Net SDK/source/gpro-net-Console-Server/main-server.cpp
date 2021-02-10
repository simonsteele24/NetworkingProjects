/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	main-server.c/.cpp
	Main source for console server application.
*/

#include "gpro-net/gpro-net.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>

#include "RakNet/MessageIdentifiers.h"
#include "RakNet/RakPeerInterface.h"

#include "RakNet//GetTime.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID

using namespace RakNet;

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

#pragma pack(push, 1)
struct networkedTime
{
	unsigned char useTimeStamp; // Assign ID_TIMESTAMP to this
	RakNet::Time timeStamp; // Put the system time in here returned by RakNet::GetTime()
	unsigned char typeId; // You should put here an enum you defined after the last one defined in MessageIdentifiers.h, lets say ID_SET_TIMED_MINE
	float x, y, z; // Mine position
	NetworkID networkId; // NetworkID of the mine, used as a common method to refer to the mine on different computers
	SystemAddress systemAddress; // The SystenAddress of the player that owns the mine
};
#pragma pack(pop)

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_INTRODUCTION_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_QUIT_MESSAGE = ID_USER_PACKET_ENUM + 3,
	ID_SHUTDOWN_SERVER = ID_USER_PACKET_ENUM + 4,
	ID_SET_TIMED_MINE = ID_USER_PACKET_ENUM
};

int main(void)
{

	//char str[512];

	/* File pointer to hold reference to our file */
    FILE * fPtr;


    /* 
     * Open file in w (write) mode. 
     * "data/file1.txt" is complete path to create file
     */
    fPtr = fopen("file1.txt", "w");

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;

	bool inLoop = true;
	bool terminateFromLoop = false;

	SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);

	// TODO - Add code body here
	printf("\n\n");

	printf("Starting the server.\n");
	fclose(fPtr);
	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	while (inLoop)
	{
		if (terminateFromLoop) 
		{
			inLoop = false;
		}

		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				fprintf(fPtr,"Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				fprintf(fPtr,"Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				fprintf(fPtr,"Another client has connected.\n");

				break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("A connection is incoming.\n");
				fprintf(fPtr,"A connection is incoming.\n");

				//write message out to client
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				bsOut.Write("Welcome to the chatroom! \n 0 - Quit the Server\n");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
				
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				fprintf(fPtr,"The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
			{
				printf("A client has disconnected.\n");
				fprintf(fPtr,"A client has disconnected.\n");
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_QUIT_MESSAGE);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
				break;
			case ID_CONNECTION_LOST:
				printf("A client lost the connection.\n");
				fprintf(fPtr,"A client lost the connection.\n");
				break;
			case ID_QUIT_MESSAGE:
			{	
				RakNet::RakString rs;
				RakNet::Time ts;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(ts);
				printf("%" PRINTF_64_BIT_MODIFIER "u ", ts);
				bsIn.Read(rs);
				char finalStr[] = "> ";
				strcat(finalStr, rs);
				strcat(finalStr, " has left :(\n");
				printf(finalStr);
				fprintf(fPtr,finalStr);

				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_QUIT_MESSAGE);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);

				terminateFromLoop = true;
			}
			break;
			case ID_GAME_MESSAGE_1:
			{
				//in
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());			
				fprintf(fPtr,"%s\n", rs.C_String());
			}

			case ID_TIMESTAMP:
			{
				RakNet::RakString rs;
				RakNet::Time ts;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(ts);
				printf("%" PRINTF_64_BIT_MODIFIER "u ",ts);
				fprintf(fPtr,"%" PRINTF_64_BIT_MODIFIER "u ",ts);
			
			}
			case ID_INTRODUCTION_MESSAGE:
			{
				RakNet::RakString rs;
				RakNet::Time ts;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(ts);
				printf("%" PRINTF_64_BIT_MODIFIER "u ", ts);
				fprintf(fPtr,"%" PRINTF_64_BIT_MODIFIER "u ", ts);
				bsIn.Read(rs);
				char finalStr[] = "> ";
				strcat(finalStr,rs);
				strcat(finalStr, " has entered the chat!\n");
				printf(finalStr);
				fprintf(fPtr,finalStr);
			}

			case ID_SHUTDOWN_SERVER:
				break;

			break;

			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				fprintf(fPtr,"Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}

	printf("Server Shutting Down\n");
	fprintf(fPtr,"Server Shutting Down\n");

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}