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
#include <conio.h> 
#include <ctime>

#include "RakNet/MessageIdentifiers.h"
#include "RakNet/RakPeerInterface.h"

#include "RakNet//GetTime.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID

using namespace RakNet;

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

// A linked list node 
struct UserDicNode {
	char key[512];
	RakNet::SystemAddress val;
	struct UserDicNode* next;
	struct UserDicNode* previous;
};

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_INTRODUCTION_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_QUIT_MESSAGE = ID_USER_PACKET_ENUM + 3,
	ID_SHUTDOWN_SERVER = ID_USER_PACKET_ENUM + 4,
	ID_CLIENT_MESSAGE = ID_USER_PACKET_ENUM + 5,
	ID_BROADCAST_MESSAGE = ID_USER_PACKET_ENUM + 6,
	ID_GET_USERS = ID_USER_PACKET_ENUM + 7,
	ID_SET_TIMED_MINE = ID_USER_PACKET_ENUM
};

UserDicNode* FindUser(UserDicNode* traversalNode,char user[])
{
	int value;
	if (traversalNode != NULL)
	{
		value = strcmp(traversalNode->key, user);
		if (value == 0)
		{
			return traversalNode;
		}
		traversalNode = traversalNode->next;
		FindUser(traversalNode->next, user);
	}
	return new UserDicNode();

}

void RemoveUser(UserDicNode* traversalNode, char user[], int & dictSize)
{
	UserDicNode* tempNode = traversalNode;

	if (traversalNode == tempNode)
	{
		memset(tempNode->key, 0, 512);
		tempNode->val = SystemAddress();
		dictSize--;
	}

	while (tempNode->next != NULL) 
	{
		int value = strcmp(traversalNode->key, user);
		if (value == 0)
		{
			tempNode->previous->next = tempNode->next;
			tempNode = nullptr;
			return;
		}
		tempNode = tempNode->next;
	}

	
	tempNode = nullptr;
	dictSize--;
}

void ShiftUsers(UserDicNode* traversalNode, UserDicNode* targetNode)
{
	if (traversalNode != NULL)
	{
		traversalNode = targetNode;
		//ShiftUsers(traversalNode)
	}
}
int main(void)
{

	//char str[512];

	const int LETTER_TO_REPRESENT_SHUTDOWN_SERVER = 27;

	/* File pointer to hold reference to our file */
    FILE * fPtr;

	UserDicNode* userDicNode = new UserDicNode();
	int dictSize = 0;
    /* 
     * Open file in w (write) mode. 
     * "data/file1.txt" is complete path to create file
     */
    fPtr = fopen("chatLog.txt", "w");

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;

	bool inLoop = true;
	bool terminateFromLoop = false;

	SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);

	// TODO - Add code body here
	printf("\n\n");

	printf("Starting the server.\n");
	
	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	while (inLoop)
	{
		if (terminateFromLoop) 
		{
			inLoop = false;
		}

		if (_kbhit()) 
		{
			char letter = _getch();

			if ((int)(letter) == LETTER_TO_REPRESENT_SHUTDOWN_SERVER) 
			{
				UserDicNode* traversalNode = userDicNode;
				RakNet::BitStream bsOut;
				while (traversalNode != NULL) 
				{
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_QUIT_MESSAGE);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, traversalNode->val, false);
					traversalNode = traversalNode->next;
				}
				
				while (traversalNode != NULL)
				{
					if (traversalNode->next != NULL) 
					{
						traversalNode = traversalNode->next;
						traversalNode->previous = nullptr;
					}
					else 
					{
						traversalNode = nullptr;
					}
				}
				inLoop = false;
			}
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
				//fprintf(fPtr,"A connection is incoming.\n");

				//write message out to client
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				bsOut.Write("Welcome to the chatroom! \n 0 - Quit the Server\n 1 - Send message \n 2 - Recieve Messages \n 3 - List All Users");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
				
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				//fprintf(fPtr,"The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
			{
				printf("A client has disconnected.\n");
				//fprintf(fPtr,"A client has disconnected.\n");
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_QUIT_MESSAGE);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
				break;
			case ID_CONNECTION_LOST:
				printf("A client lost the connection.\n");
				//fprintf(fPtr,"A client lost the connection.\n");
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
				char finalStr[500] = "> ";
				strcat(finalStr, rs);
				strcat(finalStr, " has left :(\n");
				printf(finalStr);
				fprintf(fPtr,finalStr);

				char user[512];
				RemoveUser(userDicNode, strcpy(user, rs), dictSize);

				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_QUIT_MESSAGE);

				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
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
			break;
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
			break;
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


				//Create a node and add to it the key and value
				if (dictSize == 0)
				{
					strcpy(userDicNode->key, rs.C_String());
					userDicNode->val = packet->systemAddress;
					dictSize++;
				}
				else
				{
					bool hasFoundEnd = false;
					UserDicNode* traversalNode = userDicNode;

					while (!hasFoundEnd)
					{
						if (traversalNode->next == NULL) 
						{
							hasFoundEnd = true;
							traversalNode->next = new UserDicNode();
							strcpy(traversalNode->next->key,rs.C_String());
							traversalNode->next->val = packet->systemAddress;
							traversalNode->next->previous = traversalNode;
							dictSize++;
						}
						else 
						{
							traversalNode = traversalNode->next;
						}
					}
				}

				char finalStr[500] = "> ";
				strcat(finalStr,rs);
				strcat(finalStr, " has entered the chat!\n");
				printf(finalStr);
				fprintf(fPtr,finalStr);

				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);
				bsOut.Write(finalStr);
				bsOut.Write(ts);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
			break;
			case ID_SHUTDOWN_SERVER:
				break;
			case ID_CLIENT_MESSAGE:
			{
				int value;
				char finalStr[] = "> ";
				char server[512];
				char designation[512];

				//Reading in-----------------
				RakNet::RakString rs;
				RakNet::Time ts;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				bsIn.Read(rs);


				strcpy(designation, rs);

				//Timestamp
				bsIn.Read(ts);
				printf("%" PRINTF_64_BIT_MODIFIER "u ", ts);
				fprintf(fPtr, "%" PRINTF_64_BIT_MODIFIER "u ", ts);
				bsIn.Read(rs);


				//Name
				strcat(finalStr, rs);
				//Message
				strcat(finalStr, " says: ");

				bsIn.Read(rs);
				strcat(finalStr, rs);
				strcat(finalStr, "\n");

				//Final Printing 
				printf(finalStr);
				fprintf(fPtr, finalStr);

				//--------- Writing Out
				RakNet::BitStream bsOut;


				strcpy(server, "public");
				value = strcmp(designation, server);
				//IF message is to Server this runs
				if (value == 0)
				{
					bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);

					bsOut.Write(finalStr);
					peer->SetOccasionalPing(true);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, true);
				}
				else
				{
					//Ideally this is dm
					bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);

					bsOut.Write(finalStr);
					peer->SetOccasionalPing(true);
					UserDicNode* yeeee;
					yeeee = FindUser(userDicNode, designation);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, yeeee->val, false); 

				}
					//Message to specific, this is where yo would loop from find user
			}
				break;

			case ID_GET_USERS:
			{
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GET_USERS);

				char finalStr[500] = "\n";

				UserDicNode* traversalNode = userDicNode;

				while (traversalNode != NULL)
				{
					strcat(finalStr, traversalNode->key);
					strcat(finalStr, "\n");
					traversalNode = traversalNode->next;
				}

				bsOut.Write(finalStr);
				//
				peer->SetOccasionalPing(true);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);

			}
			break;

			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				//fprintf(fPtr,"Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}


	printf("Server Shutting Down\n");
	//fprintf(fPtr,"Server Shutting Down\n");
	fclose(fPtr);
	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}