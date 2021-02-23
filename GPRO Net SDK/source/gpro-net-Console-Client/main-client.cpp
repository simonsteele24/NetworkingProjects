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
#include "gpro-net/gpro-net-common/gpro-net-console.h"

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

//List of all the game messages
enum GameMessages
{
	ID_NEW_CONNECTION = ID_USER_PACKET_ENUM + 1,
	ID_INTRODUCTION_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_QUIT_MESSAGE = ID_USER_PACKET_ENUM + 3,
	ID_SHUTDOWN_SERVER = ID_USER_PACKET_ENUM + 4,
	ID_CLIENT_MESSAGE = ID_USER_PACKET_ENUM + 5,
	ID_BROADCAST_MESSAGE = ID_USER_PACKET_ENUM + 6,
	ID_GET_USERS = ID_USER_PACKET_ENUM + 7,
	ID_JOIN_BLACKJACK = ID_USER_PACKET_ENUM + 8,
	ID_HIT = ID_USER_PACKET_ENUM + 9,
	ID_STAND = ID_USER_PACKET_ENUM + 10,
	ID_LEAVE_LOBBY = ID_USER_PACKET_ENUM + 11,
};

//This just takes input for the player when selecting what to do
int checkForInput() 
{
	char input;
	printf("\n");
	printf("Command: ");
	std::cin >> input;
	return (int)(input) - '0';
}

//This just takes input for the player when selecting what to do
int checkForInputGame()
{
	char input;
	printf("\n");
	printf("Game Command: ");
	std::cin >> input;
	return (int)(input)-'0';
}

int main(void)
{
	//Local Variables
	char str[512] = "";
	char username[512] = "";
	
	bool inGame = false;
	bool isConnected = false;
	bool inLoop = true;
	bool canRecieveInput = false;
	bool terminateFromLoop = false;
	int inputNum = 0;

	//Raknet Variables
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();

	RakNet::Packet* packet = NULL;

	RakNet::SystemAddress address = SystemAddress();

	SocketDescriptor sd = SocketDescriptor();;

	peer->Startup(1, &sd, 1);

	//Print out all of the appriopriate intro prints and then take input of the address and
	// username that the user will type in.
	printf("\n\n");

	printf("Starting the client.\n");
	
	printf("\n\n");

	printf("Enter your IP address: ");

	std::cin >> str;

	printf("\n\n");

	printf("Enter your Username: ");

	std::cin >> username;

	peer->Connect(str, SERVER_PORT, 0, 0);

	// Main Loop
	while (inLoop)
	{
		while (inGame)
		{
			//User input for decisions on what to do(Menu) ------------
			if (canRecieveInput)
			{
				inputNum = checkForInputGame();

				//Check the input 0 for hitting, 1 for standing, 2 for writing message, 3 to receive message, 4 to quit.
				switch (inputNum)
				{
				case 0:
				{
					//hitting
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_HIT);
					bsOut.Write(username);
					peer->SetOccasionalPing(true);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
					break;
				}
				case 1:
				{
					//standing
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_STAND);
					bsOut.Write(username);
					peer->SetOccasionalPing(true);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
					break;
				}
				case 2:
				{
					//Recieve anythning and then write
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
				//receive messages
				case 3:
				{
					//Receive packet and the switch deals with what type of message is being read.
					for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
					{
						switch (packet->data[0])
						{
							//When the client has been accepted 
						case ID_CONNECTION_REQUEST_ACCEPTED:
						{
							printf("Our connection request has been accepted.\n");

							// Use a BitStream to write a custom user message
							// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
							RakNet::BitStream bsOut;

							//Write out to the ID_INTRODUCTION_MESSAGE and time then username. set ping again to keep accuracy
							bsOut.Write((RakNet::MessageID)ID_INTRODUCTION_MESSAGE);
							bsOut.Write(RakNet::GetTimeUS() / 1000);
							bsOut.Write(username);
							peer->SetOccasionalPing(true);

							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						}
						break;
						//Message left server and then set inloop to false which will break out.
						case ID_QUIT_MESSAGE:
							printf("You have left the server");
							inLoop = false;
							break;
							//This will read in message and then allow input
						case ID_NEW_CONNECTION:
						{
							RakNet::RakString rs = RakString();
							RakNet::BitStream bsIn(packet->data, packet->length, false);
							bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
							bsIn.Read(rs);
							printf("\n");
							printf("%s\n", rs.C_String());

							//enable input for the menu and set address
							canRecieveInput = true;
							address = packet->systemAddress;
						}
						break;
						// This will read and print out any broadcasted message from the server(and/or clients)
						case ID_BROADCAST_MESSAGE:
						{
							RakNet::RakString rs = RakString();
							RakNet::BitStream bsIn(packet->data, packet->length, false);
							bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
							bsIn.Read(rs);
							printf(rs.C_String());
						}
						break;

						//Reads in the data from the server of the list of users
						case ID_GET_USERS:
						{
							RakNet::RakString rs = RakString();
							RakNet::BitStream bsIn(packet->data, packet->length, false);
							bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
							bsIn.Read(rs);
							printf("%s\n", rs.C_String());
							break;
						}
						//Default will just do nothings
						default:
							break;
						}
					}
					break;
				}

				//Quit message
				case 4:
				{
					gpro_consoleClear();
					printf("Press 2 to confirm and see lobby menu");
					//Receive packet and write out bitsream calling the quit message and the gettime. Then write the username
					//The SetOccasionalPing is for maintaining accuracy
					packet = peer->Receive();
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_LEAVE_LOBBY);
					bsOut.Write(RakNet::GetTimeUS() / 1000);
					bsOut.Write(username);

					peer->SetOccasionalPing(true);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
					inGame = false;			
					break;
				}
				default:
					printf("Invalid Input \n");
					break;
				}
			}
			// ------------
			// If the user is now just receiving data packets and not typing, then these run
			else
			{
			//Receive packet and the switch deals with what type of message is being read.
			for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
			{
				switch (packet->data[0])
				{
					//When the client has been accepted 
				case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					printf("Our connection request has been accepted.\n");

					// Use a BitStream to write a custom user message
					// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
					RakNet::BitStream bsOut;

					//Write out to the ID_INTRODUCTION_MESSAGE and time then username. set ping again to keep accuracy
					bsOut.Write((RakNet::MessageID)ID_INTRODUCTION_MESSAGE);
					bsOut.Write(RakNet::GetTimeUS() / 1000);
					bsOut.Write(username);
					peer->SetOccasionalPing(true);

					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				}
				break;
				//Message left server and then set inloop to false which will break out.
				case ID_QUIT_MESSAGE:
					printf("You have left the server");
					inLoop = false;
					break;
					//This will read in message and then allow input
				case ID_NEW_CONNECTION:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("\n");
					printf("%s\n", rs.C_String());

					//enable input for the menu and set address
					canRecieveInput = true;
					address = packet->systemAddress;
				}
				break;
				// This will read and print out any broadcasted message from the server(and/or clients)
				case ID_BROADCAST_MESSAGE:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf(rs.C_String());
				}
				break;

				//Reads in the data from the server of the list of users
				case ID_GET_USERS:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
					break;
				}
				//Reads in the data from the server of the list of users
				case ID_JOIN_BLACKJACK:
				{
					gpro_consoleClear();
					inGame = true;
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
					break;
				}

				case ID_HIT:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
					break;
				}

				case ID_STAND:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
					break;
				}

				case ID_LEAVE_LOBBY:
				{
					gpro_consoleClear();
					inGame = false;
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
				}
				//Default will just do nothings
				default:
					break;
				}
			}
			}

		}
		//User input for decisions on what to do(Menu lobby) ------------
		if (canRecieveInput) 
		{
			inputNum = checkForInput();

			//Check the input 0 for quitting, 1 for writing a message, 2 to recieve messages/update, 3 to get list of users, 4 to join blackjack
			switch (inputNum)
			{
			case 0:
			{
				//Receive packet and write out bitsream calling the quit message and the gettime. Then write the username
				//The SetOccasionalPing is for maintaining accuracy
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
				//Receive packet and write out bitsream calling the quit message and the gettime. Then write the username
				//The SetOccasionalPing is for maintaining accuracy
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
				//Receive packet and the switch deals with what type of message is being read.
				for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
				{
					switch (packet->data[0])
					{
					//When the client has been accepted 
					case ID_CONNECTION_REQUEST_ACCEPTED:
					{
						printf("Our connection request has been accepted.\n");

						// Use a BitStream to write a custom user message
						// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
						RakNet::BitStream bsOut;

						//Write out to the ID_INTRODUCTION_MESSAGE and time then username. set ping again to keep accuracy
						bsOut.Write((RakNet::MessageID)ID_INTRODUCTION_MESSAGE);
						bsOut.Write(RakNet::GetTimeUS() / 1000);
						bsOut.Write(username);
						peer->SetOccasionalPing(true);

						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					}
					break;
					//Message left server and then set inloop to false which will break out.
					case ID_QUIT_MESSAGE:
						printf("You have left the server");
						inLoop = false;
						break;
					//This will read in message and then allow input
					case ID_NEW_CONNECTION:
					{
						RakNet::RakString rs = RakString();
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("\n");
						printf("%s\n", rs.C_String());

						//enable input for the menu and set address
						canRecieveInput = true;
						address = packet->systemAddress;
					}
					break;
					// This will read and print out any broadcasted message from the server(and/or clients)
					case ID_BROADCAST_MESSAGE:
					{
						RakNet::RakString rs = RakString();
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf(rs.C_String());
					}
					break;
					
					//Reads in the data from the server of the list of users
					case ID_GET_USERS:
					{
						RakNet::RakString rs = RakString();
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("%s\n", rs.C_String());
						break;
					}
					//Reads in the data from the server of the list of users
					case ID_JOIN_BLACKJACK:
					{
						gpro_consoleClear();
						inGame = true;
						RakNet::RakString rs = RakString();
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("%s\n", rs.C_String());
						bsIn.Read(rs);
						printf("%s\n", rs.C_String());
						break;
					}
					//Default will just do nothings
					default:
						break;
					}
				}
				break;
			}
			//This will request get list of users from server
			case 3:
			{
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GET_USERS);

				peer->SetOccasionalPing(true);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
				break;
			}

			//This will request ability to join blackjack room
			case 4:
			{
				system("clear");
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_JOIN_BLACKJACK);

				printf("Attempting to join Standby...");
				
				peer->SetOccasionalPing(true);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
				break;
			}
			default:
				printf("Invalid Input \n");
				break;
			}
		}
		// ------------
		// If the user is now just receiving data packets and not typing, then these run
		else 
		{
		//Recieve packet
			for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
			{
				switch (packet->data[0])
				{
				//When the client has been accepted 
				case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					printf("Our connection request has been accepted.\n");

					// Use a BitStream to write a custom user message
					// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
					RakNet::BitStream bsOut;

					bsOut.Write((RakNet::MessageID)ID_INTRODUCTION_MESSAGE);

					bsOut.Write(RakNet::GetTimeUS() / 1000);
					bsOut.Write(username);
					peer->SetOccasionalPing(true);

					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				}
				break;
				//Message left server and then set inloop to false which will break out.
				case ID_QUIT_MESSAGE:
					printf("You have left the server");
					inLoop = false;
					break;
				//This will read in message and then allow input
				case ID_NEW_CONNECTION:
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
				// This will read and print out any broadcasted message from the server(and/or clients)
				case ID_BROADCAST_MESSAGE:
				{
					RakNet::RakString rs = RakString();
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf(rs.C_String());
				}
				break;
				//Reads in the data from the server of the list of users
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
		}

		
	}

	//End of the line just destroy
	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}
