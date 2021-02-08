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

	main-client.c/.cpp
	Main source for console client application.
*/

#include "gpro-net/gpro-net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <time.h>

#include "RakNet/MessageIdentifiers.h"

#include "RakNet/RakPeerInterface.h"

#include "RakNet//GetTime.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID

using namespace RakNet;

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_SET_TIMED_MINE = ID_USER_PACKET_ENUM
};


int main(void)
{
	char str[512];

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();

	RakNet::Packet* packet;

	SocketDescriptor sd;

	peer->Startup(1, &sd, 1);

	// TODO - Add code body here
	printf("\n\n");
	
	printf("Starting the client.\n");
	
	printf("\n\n");

	printf("Enter your IP address: ");

	std::cin >> str;

	peer->Connect(str, SERVER_PORT, 0, 0);

	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
				/*	case ID_REMOTE_DISCONNECTION_NOTIFICATION:
						printf("Another client has disconnected.\n");
						break;
					case ID_REMOTE_CONNECTION_LOST:
						printf("Another client has lost the connection.\n");
						break;
					case ID_REMOTE_NEW_INCOMING_CONNECTION:
						printf("Another client has connected.\n");
						break;*/
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");

				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				RakNet::BitStream bsOut;
				//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				time_t giveTime = time(NULL);

				MessageID useTimeStamp; // Assign this to ID_TIMESTAMP
				RakNet::Time timeStamp; // Put the system time in here returned by RakNet::GetTime()
				MessageID typeId; // This will be assigned to a type I've added after ID_USER_PACKET_ENUM, lets say ID_SET_TIMED_MINE
				useTimeStamp = ID_TIMESTAMP;
				timeStamp = RakNet::GetTime();
				typeId = ID_SET_TIMED_MINE;

				bsOut.Write((RakNet::MessageID)ID_SET_TIMED_MINE);
				//bsOut.Write(useTimeStamp);
				//bsOut.Write(timeStamp);
				//bsOut.Write(typeId);

				// Trying to figure out timestamps, atm to get system time you can use this line below.
				bsOut.Write(ctime(&giveTime));
				//printf("%s", ctime(&giveTime)); //ctime() returns given time
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:

				printf("We have been disconnected.\n");

				break;
			case ID_CONNECTION_LOST:

				printf("Connection lost.\n");
				break;

			case ID_GAME_MESSAGE_1:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());
			}
			break;
			case ID_SET_TIMED_MINE:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());
			}
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
			
		}
	}
	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}

//int main(int const argc, char const* const argv[])
//{
//
//
//	printf("\n\n");
//	system("pause");
//}
