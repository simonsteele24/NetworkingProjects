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

	main-server.c/.cpp
	Main source for console server application.
*/

#include "gpro-net/gpro-net.h"
#include "gpro-net/gpro-net-common/gpro-net-console.h"
#include "gpro-net/BlackjackGamemanager.h"
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

// Clients and Server Pots
#define MAX_CLIENTS 10
#define SERVER_PORT 60000

// A linked list node to store all users
struct UserDicNode {
	char key[512] = "";
	RakNet::SystemAddress val;
	struct UserDicNode* next = NULL;
	struct UserDicNode* previous = NULL;
};

// Enum for custom Broadcast Messages
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
	ID_PLAYER_TURN = ID_USER_PACKET_ENUM + 12,
	ID_RETURN_BLACKJACK_RESULTS = ID_USER_PACKET_ENUM + 13,
	ID_SET_TIMED_MINE = ID_USER_PACKET_ENUM
};

// Finds the user based on given character. Used to find a user for broadcasting a private message
UserDicNode* FindUser(UserDicNode* traversalNode,char user[])
{
	UserDicNode* tempNode = traversalNode;
	int value;

	// Traverse through linked list until given character is found
	while (tempNode != NULL)
	{
		value = strcmp(tempNode->key, user);
		if (value == 0)
		{
			return tempNode;
		}
		tempNode = tempNode->next;
	}

	// If given character isn't found, then return nothing
	return new UserDicNode();
}

// Removes a user based on given character. Used to remove a user when the user leaves
void RemoveUser(UserDicNode* traversalNode, char user[], int & dictSize)
{
	UserDicNode* tempNode = traversalNode;

	// Check if traversal node is the head. Then remove it linked list
	if (traversalNode == tempNode)
	{
		memset(tempNode->key, 0, 512);
		tempNode->val = SystemAddress();
		dictSize--;
	}

	// Traverse through linked list until given character is found
	while (tempNode->next != NULL) 
	{
		int value = strcmp(traversalNode->key, user);
		if (value == 0)
		{
			// Remove from linked list
			tempNode->previous->next = tempNode->next;
			tempNode = nullptr;
			return;
		}
		tempNode = tempNode->next;
	}

	// Fully remove head if everything else fails
	tempNode = nullptr;
	dictSize--;
}

int main(void)
{
	BlackjackGamemanager manager = BlackjackGamemanager();

	// Constant to represent escape to shut down the server
	const int LETTER_TO_REPRESENT_SHUTDOWN_SERVER = 27;

	// File pointer to hold reference to our file
    FILE * fPtr;
	fPtr = fopen("chatLog.txt", "w");

	// Values to control linked list 
	UserDicNode* userDicNode = new UserDicNode();
	UserDicNode* playerToGo = new UserDicNode();
	int dictSize = 0;

	// Server vals for packet and recieving packets
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet = NULL;

	// Booleans to know when to leave server loop
	bool inLoop = true;
	bool terminateFromLoop = false;
	bool awaitingOnInput = false;

	// Startup Server
	SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);

	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	while (inLoop)
	{
		// Check if networking loop needs to be terminated
		if (terminateFromLoop) 
		{
			inLoop = false;
		}

		if (manager.GetPlayerCount() != 0 && !awaitingOnInput) 
		{
			if (!manager.CheckIfGameHasBeenInitialized()) 
			{
				manager.InitializeHand();
			}

			if (manager.CheckForEndOfGame()) 
			{
				const std::string newString = manager.DeclareWinner();
				std::vector<std::string> playerNames = manager.GetPlayerNames();
				for (int i = 0; i < playerNames.size(); i++) 
				{
					std::string s = manager.GetNextPlayer();
					char char_array[502];
					strcpy(char_array, s.c_str());
					playerToGo = FindUser(userDicNode, char_array);

					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_RETURN_BLACKJACK_RESULTS);
					bsOut.Write(newString.c_str());
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, playerToGo->val, false);
				}
				manager.InitializeHand();
			}
			else 
			{
				std::string s = manager.GetNextPlayer();
				char char_array[502];
				strcpy(char_array, s.c_str());
				playerToGo = FindUser(userDicNode, char_array);

				RakNet::BitStream bsOut;
				const std::string newString = manager.PrintStatsToPlayer() + "Your turn:\n";
				bsOut.Write((RakNet::MessageID)ID_PLAYER_TURN);
				bsOut.Write(newString.c_str());
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, playerToGo->val, false);

				awaitingOnInput = true;
			}
		}
		else if (manager.GetPlayerCount() == 0) 
		{
			manager.DeInitializeGame();
			awaitingOnInput = false;
		}

		// Check for keyboard input
		if (_kbhit()) 
		{
			// Check if input matches key to shutdown server
			char letter = _getch();

			if ((int)(letter) == LETTER_TO_REPRESENT_SHUTDOWN_SERVER) 
			{
				UserDicNode* traversalNode = userDicNode;
				RakNet::BitStream bsOut;

				// Traverse linked list to disconnect all clients still on
				while (traversalNode != NULL) 
				{
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_QUIT_MESSAGE);
					//This line will broadcast a quit message to all ofc ignoring server
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, traversalNode->val, false);
					traversalNode = traversalNode->next;
				}

				// Delete linked list
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

				// Get out of loop
				terminateFromLoop = true;
				continue;
			}
		}

		// Main Networking Loop to recieve and send packets
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			// Check packet data type
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
			{
				//write out private message out to client about chatroom controls
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_NEW_CONNECTION);
				bsOut.Write("Welcome to the chatroom! \n 0 - Quit the Server\n 1 - Send message \n 2 - List All Users \n 3 - Enter BlackJack Game");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
			break;
			case ID_QUIT_MESSAGE:
			{	
				// Read out user and then display player to broadcast message
				RakNet::RakString rs = RakString();
				RakNet::Time ts = Time();
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(ts);
				printf("%" PRINTF_64_BIT_MODIFIER "u ", ts);
				fprintf(fPtr,"%" PRINTF_64_BIT_MODIFIER "u ", ts);
				bsIn.Read(rs);
				char finalStr[500] = "> ";
				strcat(finalStr, rs);
				strcat(finalStr, " has left :(\n");
				printf(finalStr);
				fprintf(fPtr,finalStr);

				// Remove given user from server
				char user[512] = "";
				RemoveUser(userDicNode, strcpy(user, rs), dictSize);

				// Broadcast to server user is leaving
				RakNet::BitStream broadCastOut;
				broadCastOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);
				broadCastOut.Write(finalStr);
				peer->Send(&broadCastOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, true);

				// Send message to player that they have the ok to quit
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_QUIT_MESSAGE);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
			break;
			case ID_INTRODUCTION_MESSAGE:
			{
				RakNet::RakString rs = RakString();
				RakNet::Time ts = Time();
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(ts);

				// Write out time stamp to server
				printf("%" PRINTF_64_BIT_MODIFIER "u ", ts);
				fprintf(fPtr,"%" PRINTF_64_BIT_MODIFIER "u ", ts);
				bsIn.Read(rs);


				// Make new user head if no head exist
				if (dictSize == 0)
				{
					strcpy(userDicNode->key, rs.C_String());
					userDicNode->val = packet->systemAddress;
					dictSize++;
				}
				else
				{
					// Add user to end of linked list if there is already a head to the list
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

				// Create body of message
				char finalStr[512] = "> ";
				strcat(finalStr,rs);
				strcat(finalStr, " has entered the chat!\n");
				printf(finalStr);
				fprintf(fPtr,finalStr);

				// Broadcast to ALL users that new user has joined
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);
				bsOut.Write(finalStr);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, true);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
			}
			break;
			case ID_CLIENT_MESSAGE:
			{
				int value = 0;
				char finalStr[512] = "> ";
				char dmMessage[512] = "> ";
				char name[512] = "";
				char server[512] = "";
				char designation[512] = "";

				//Reading in-----------------
				RakNet::RakString rs = RakString();
				RakNet::Time ts = Time();
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
				strcat(dmMessage, rs);
				// Copy name
				strcpy(name, rs.C_String());
				//Message
				strcat(dmMessage, " says: ");

				bsIn.Read(rs);
				strcat(dmMessage, rs);
				strcat(dmMessage, "\n");

				//--------- Writing Out
				RakNet::BitStream bsOut;


				strcpy(server, "public");
				value = strcmp(designation, server);

				//IF message is to Server this runs
				if (value == 0)
				{
					// Universal server broadcast

					//Name
					strcat(finalStr, name);
					//Message
					strcat(finalStr, " says to server: ");

					strcat(finalStr, rs);
					strcat(finalStr, "\n");

					//Final Printing 
					printf(finalStr);
					fprintf(fPtr, finalStr);

					bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);
					bsOut.Write(dmMessage);
					peer->SetOccasionalPing(true);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, true);
				}
				else
				{
					// DM to certain user

					// Find user
					UserDicNode* yeeee = NULL;
					yeeee = FindUser(userDicNode, designation);

					if (yeeee != NULL) 
					{
						strcat(finalStr, name);
						strcat(finalStr, " says to user ");
						strcat(finalStr, yeeee->key);
						strcat(finalStr, ": ");
						strcat(finalStr, rs);
						strcat(finalStr, "\n");
						printf(finalStr);
						fprintf(fPtr, finalStr);

						// Send packet to user (if valid)
						bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);
						bsOut.Write(dmMessage);
						peer->SetOccasionalPing(true);
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, yeeee->val, false);
					}
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

				// Traverse all of list to add to message
				while (traversalNode != NULL)
				{
					strcat(finalStr, traversalNode->key);
					strcat(finalStr, "\n");
					traversalNode = traversalNode->next;
				}
				bsOut.Write(finalStr);
				
				// Send to system that requested it
				peer->SetOccasionalPing(true);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);

			}
			break;
			case ID_JOIN_BLACKJACK:
			{
				//write out private message out to client about chatroom controls
				RakNet::RakString rs = RakString();
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				manager.AddInPlayer(rs.C_String());

				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_JOIN_BLACKJACK);
				bsOut.Write("Welcome to the BJ room");
				bsOut.Write("\n 0 - Hit\n 1 - Stand \n 2 - Send Message \n 3 - Quit");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
				break;
			}

			case ID_HIT:
			{
				manager.HitPlayer();

				//write out private message out to client about chatroom controls
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);
				bsOut.Write(manager.PrintStatsToPlayer().c_str());
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
				awaitingOnInput = false;
				break;
			}

			case ID_STAND:
			{
				manager.StayPlayer();

				//write out private message out to client about chatroom controls
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);
				bsOut.Write("Player is standing");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
				awaitingOnInput = false;
				break;
			}

			case ID_LEAVE_LOBBY:
			{
				manager.RemovePlayer(playerToGo->key);
				//write out private message out to client about chatroom controls
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_BROADCAST_MESSAGE);
				bsOut.Write("Welcome to the chatroom! \n 0 - Quit the Server\n 1 - Send message \n 2 - List All Users \n 3 - Enter BlackJack Game");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);

				break;
			}

			default:
				break;
			}
		}
	}
	system("pause");
	fclose(fPtr);
	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}