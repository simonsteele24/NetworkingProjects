/*
   Copyright 2021 Simon Steele & Chun Tao Lin

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
	By Simon Steele & Chun Tao Lin

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
	ID_INTRODUCTION_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_QUIT_MESSAGE = ID_USER_PACKET_ENUM + 3,
	ID_SHUTDOWN_SERVER = ID_USER_PACKET_ENUM + 4,
	ID_CLIENT_MESSAGE = ID_USER_PACKET_ENUM + 5,
	ID_BROADCAST_MESSAGE = ID_USER_PACKET_ENUM + 6,
	ID_GET_USERS = ID_USER_PACKET_ENUM + 7,
};

int checkForInput() 
{
	char input;
	printf("Command: ");
	std::cin >> input;
	return (int)(input) - '0';
}

int main(void)
{
	char str[512] = "";
	char username[512] = "";

	bool isConnected = false;
	bool inLoop = true;
	bool canRecieveInput = false;
	bool terminateFromLoop = false;
	int inputNum = 0;

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();

	RakNet::Packet* packet = NULL;

	RakNet::SystemAddress address = SystemAddress();

	SocketDescriptor sd = SocketDescriptor();;

	peer->Startup(1, &sd, 1);

	// TODO - Add code body here
	printf("\n\n");
	
	printf("Starting the client.\n");
	
	printf("\n\n");

	printf("Enter your IP address: ");

	std::cin >> str;

	printf("\n\n");

	printf("Enter your Username: ");

	std::cin >> username;

	peer->Connect(str, SERVER_PORT, 0, 0);

	while (inLoop)
	{
		if (canRecieveInput) 
		{
			inputNum = checkForInput();

			switch (inputNum)
			{
			case 0:
			{
				packet = peer->Receive();
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_QUIT_MESSAGE);
				bsOut.Write(RakNet::GetTimeUS() / 1000);
				bsOut.Write(username);

				peer->SetOccasionalPing(true);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
				break;
			}
			case 1:
			{
				packet = peer->Receive();
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_CLIENT_MESSAGE);

				//Designation
				printf("Whom is the message for? Type the UserName(dm) or public: ");
				std::cin >> str;
				bsOut.Write(str);

				//Timestamp
				bsOut.Write(RakNet::GetTimeUS() / 1000);

				//Username
				bsOut.Write(username);

				//Message
				printf("What message would you like to send: ");
				std::cin >> str;
				bsOut.Write(str);

				peer->SetOccasionalPing(true);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
				break;
			}
			case 2:
			{
				for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
				{
					switch (packet->data[0])
					{
					case ID_CONNECTION_REQUEST_ACCEPTED:
					{
						printf("Our connection request has been accepted.\n");

						// Use a BitStream to write a custom user message
						// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
						RakNet::BitStream bsOut;
						time_t giveTime = time(NULL);

						bsOut.Write((RakNet::MessageID)ID_INTRODUCTION_MESSAGE);

						bsOut.Write(RakNet::GetTimeUS() / 1000);
						bsOut.Write(username);
						peer->SetOccasionalPing(true);

						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					}
					break;
					case ID_QUIT_MESSAGE:
						printf("You have left the server");
						inLoop = false;
						break;
					case ID_GAME_MESSAGE_1:
					{
						RakNet::RakString rs = RakString();
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("\n");
						printf("%s\n", rs.C_String());

						canRecieveInput = true;
						address = packet->systemAddress;
					}
					break;
					case ID_BROADCAST_MESSAGE:
					{
						RakNet::RakString rs = RakString();
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf(rs.C_String());
					}
					break;
					case ID_TIMESTAMP:
					{
						RakNet::RakString rs = RakString();
						RakNet::Time ts = Time();
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(ts);
						printf("%s\n", rs.C_String());
						printf("%" PRINTF_64_BIT_MODIFIER "u ", ts);
						break;
					}
					break;
					case ID_GET_USERS:
					{
						RakNet::RakString rs = RakString();
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("%s\n", rs.C_String());

						break;
					}

					default:
						break;
					}
				}
				break;
			}
			case 3:
			{
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GET_USERS);

				peer->SetOccasionalPing(true);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
				break;
			}
			default:
				printf("Invalid Input \n");
				break;
			}
		}
		else 
		{
			for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
			{
				switch (packet->data[0])
				{
				case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					printf("Our connection request has been accepted.\n");

					// Use a BitStream to write a custom user message
					// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
					RakNet::BitStream bsOut;
					time_t giveTime = time(NULL);

					bsOut.Write((RakNet::MessageID)ID_INTRODUCTION_MESSAGE);

					bsOut.Write(RakNet::GetTimeUS() / 1000);
					bsOut.Write(username);
					peer->SetOccasionalPing(true);

					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				}
				break;
				case ID_QUIT_MESSAGE:
					printf("You have left the server");
					inLoop = false;
					break;
				case ID_GAME_MESSAGE_1:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("\n");
					printf("%s\n", rs.C_String());

					canRecieveInput = true;
					address = packet->systemAddress;
				}
				break;
				case ID_BROADCAST_MESSAGE:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf(rs.C_String());
				}
				break;
				case ID_TIMESTAMP:
				{
					RakNet::RakString rs = RakString();
					RakNet::Time ts = Time();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(ts);
					printf("%s\n", rs.C_String());
					printf("%" PRINTF_64_BIT_MODIFIER "u ", ts);
					break;
				}
			
				case ID_GET_USERS:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
					printf("WRGRHERH");
					break;
				}

				default:
					break;
				}
			}
		}

		
	}
	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}
