#include "gpro-net/BlackjackGamemanager.h"

BlackjackGamemanager::BlackjackGamemanager()
{
	players = std::vector<Player>();
}

BlackjackGamemanager::~BlackjackGamemanager()
{
}

void BlackjackGamemanager::AddInPlayer(const std::string& _playerName) 
{
	players.push_back(Player());
	players[players.size() - 1].playerName = _playerName;
}

void BlackjackGamemanager::RemovePlayer(const std::string& _playerName) 
{
	for (int i = 0; i < players.size(); i++) 
	{
		if (players[i].playerName == _playerName) 
		{
			players.erase(players.begin() + i);
			return;
		}
	}
}

const std::string BlackjackGamemanager::GetNextPlayer() 
{
	currentPlayerIndex++;

	if (currentPlayerIndex == players.size()) 
	{
		RunDealersTurn();
		currentPlayerIndex = 0;
	}

	return players[currentPlayerIndex].playerName;
}

void BlackjackGamemanager::InitializeHand() 
{
	for (int i = 0; i < players.size(); i++) 
	{
		players[i].isStaying = false;
		players[i].cards.clear();
		players[i].cards.push_back(PickRandomCard());
		players[i].cards.push_back(PickRandomCard());
	}
	dealer.cards.clear();
	dealer.cards.push_back(PickRandomCard());

	gameHasBeenInitialized = true;
}

int BlackjackGamemanager::PickRandomCard()
{
	return rand() % 10 + 1;
}

std::string BlackjackGamemanager::PrintStatsToPlayer() 
{
	std::string output = "";
	output += "Dealer has: " + DisplayCards(dealer.cards) + "\n";
	output += "Player has: " + DisplayCards(players[currentPlayerIndex].cards) + "\n";
	return output;
}

std::string BlackjackGamemanager::DisplayCards(const std::vector<int>& cards)
{
	std::string finalResult = "";
	for (int i = 0; i < cards.size(); i++)
	{
		finalResult += std::to_string(cards[i]) + " ";
	}
	return finalResult;
}

void BlackjackGamemanager::RunDealersTurn() 
{
	dealer.cards.push_back(PickRandomCard());
}

void BlackjackGamemanager::HitPlayer() 
{
	players[currentPlayerIndex].cards.push_back(PickRandomCard());
}

void BlackjackGamemanager::StayPlayer() 
{
	players[currentPlayerIndex].isStaying = true;
}

const bool BlackjackGamemanager::CheckForEndOfGame() 
{
	for (int i = 0; i < players.size(); i++) 
	{
		if (!players[i].isStaying) 
		{
			return false;
		}
	}
	return true;
}

std::string BlackjackGamemanager::DeclareWinner() 
{
	std::string output = "\n";

	while (dealer.GetScore() < 17) 
	{
		RunDealersTurn();
	}

	for (int i = 0; i < players.size(); i++) 
	{
		output += players[i].playerName + " Got " + std::to_string(players[i].GetScore()) + "\n";
	}

	output += "Dealer Got " + std::to_string(dealer.GetScore()) + "\n";

	int dealerVal = dealer.GetScore();
	int playerVal = players[0].GetScore();

	//First check Dealer has more value than dealer
	if (dealerVal > playerVal)
	{
		//dealer busted
		if (dealerVal > 21)
		{
			//player bustsed
			if (playerVal > 21)
			{
				output += "Everyone busted, Dealer wins by default\n";
			}
			//player wins
			else
			{
				output += "Player Won\n";
			}
		}
		else
		{
			output += "Dealer Won\n";
		}
	}
	//Second check Dealer has more value than player
	else if (playerVal > dealerVal)
	{
		//dealer busted
		if (dealerVal > 21)
		{
			//player bustsed
			if (playerVal > 21)
			{
				output += "Everyone busted, Dealer wins by default\n";
			}
			//player wins
			else
			{
				output += "Player Won\n";
			}
		}
		else
		{
			output += "Player Won\n";
		}
	}
	else
	{
		output += "NO winner\n";
	}

	return output;
}

std::vector<std::string> BlackjackGamemanager::GetPlayerNames() 
{
	std::vector<std::string> names = std::vector<std::string>();

	for (int i = 0; i < players.size(); i++) 
	{
		names.push_back(players[i].playerName);
	}

	return names;
}

void BlackjackGamemanager::DeInitializeGame() 
{
	gameHasBeenInitialized = false;
}