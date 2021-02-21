// SimpleBlackJack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  

struct Player 
{
	std::vector<int> cards;
	float money, bet = 0;
};

int CalculateScore(const std::vector<int>& cards)
{
	int totalScore = 0;

	for (int i = 0; i < cards.size(); i++)
	{
		totalScore += cards[i];
	}

	return totalScore;
}

int PickRandomCard()
{
	return rand() % 10 + 1;
}

std::string DetermineWinner(Player& dealer, Player& player)
{
	std::string output;
	int dealerVal , playerVal;

	dealerVal = CalculateScore(dealer.cards);
	playerVal = CalculateScore(player.cards);
	
	//First check Dealer has more value than dealer
	if (dealerVal > playerVal)
	{
		//dealer busted
		if (dealerVal > 21)
		{
			//player bustsed
			if (playerVal > 21)
			{
				output = "Everyone busted, Dealer wins by default";
				player.money -= player.bet + dealer.bet;
				dealer.money += player.bet + dealer.bet;
				player.bet = 0;
				dealer.bet = 0;
			}
			//player wins
			else
			{
				output = "Player Won";
				player.money += player.bet + dealer.bet;
				dealer.money -= dealer.bet;
				player.bet = 0;
				dealer.bet = 0;
			}				
		}
		else
		{
			output = "Dealer Won";
			dealer.money += player.bet + dealer.bet;
			player.money -= player.bet;
			dealer.bet = 0;
			player.bet = 0;
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
				output = "Everyone busted, Dealer wins by default";
				player.money -= player.bet + dealer.bet;
				dealer.money += player.bet + dealer.bet;
				player.bet = 0;
				dealer.bet = 0;
			}
			//player wins
			else
			{
				output = "Player Won";
				player.money += player.bet + dealer.bet;
				dealer.money -= dealer.bet;
				player.bet = 0;
				dealer.bet = 0;
			}
		}
		else
		{
			output = "Dealer Won";
			dealer.money += player.bet + dealer.bet;
			player.money -= player.bet;
			dealer.bet = 0;
			player.bet = 0;
		}
	}
	else
	{
		output = "NO winner";
		dealer.bet = 0;
		player.bet = 0;
	}

	return output;
}
void PlaceBet(float& bet, float money)
{
	std::string input;
	
	std::cout << "Input Number to Bet: ";
	std::cin >> input;

	if ((float)std::stoi(input) > money)
	{
		do
		{
			std::cout << "Not Enough Money on hand.." << "\n";
			std::cout << "Input Number to Bet: ";
			std::cin >> input;
		} while ((float)std::stoi(input) > money);

	}

	if (input.size() == 0)
	{
		bet = 0;
	}
	else
		bet = (float)std::stoi(input);
}

std::string DisplayCards(const std::vector<int> & cards) 
{
	std::string finalResult = "";
	for (int i = 0; i < cards.size(); i++) 
	{
		finalResult += std::to_string(cards[i]) + " ";
	}
	return finalResult;
}

void RunPlayerInterface(std::vector<Player> & players) 
{
	bool playerIsChoosing = true;

	while (playerIsChoosing)
	{
		std::string input;
		std::cout << "Hit (0) or Stand (1): ";
		std::cin >> input;

		int choice = std::stoi(input);

		switch (choice)
		{
		case 0:
		{
			players[1].cards.push_back(PickRandomCard());

			if (CalculateScore(players[0].cards) < 17)
			{
				players[0].cards.push_back(PickRandomCard());
			}

			std::cout << "Dealer Has: " << DisplayCards(players[0].cards) << std::endl;
			std::cout << "Player Has: " << DisplayCards(players[1].cards) << std::endl;
		}
		break;
		case 1:
			playerIsChoosing = false;
			break;
		default:
			break;
		}
	}
}

void RunoutRestOfDealerInterface(std::vector<Player>& players) 
{
	int dealerScore = CalculateScore(players[0].cards);
	while (dealerScore <= 17) 
	{
		players[0].cards.push_back(PickRandomCard());
		dealerScore = CalculateScore(players[0].cards);
	}
}

int main()
{
	std::srand(time(NULL));
	//Initialize vector of players and money
	std::vector<Player> players;
	players.push_back(Player());
	players.push_back(Player());
	players[0].money = 1000;
	players[1].money = 1000;

	bool inGame = true;

	std::cout << "Place Bets Now";

	std::cout << "\n";

	PlaceBet(players[0].bet,players[0].money);	
	PlaceBet(players[1].bet, players[1].money);

	while (inGame)
	{
		players[0].cards.push_back(PickRandomCard());
		std::cout << "Dealer Has: " << DisplayCards(players[0].cards) << std::endl;

		players[1].cards.push_back(PickRandomCard());
		players[1].cards.push_back(PickRandomCard());
		std::cout << "Player Has: " << DisplayCards(players[1].cards) << std::endl;

		RunPlayerInterface(players);
		RunoutRestOfDealerInterface(players);

		std::cout << std::endl << "---Final Score---" << std::endl;
		std::cout << "Player: " << std::to_string(CalculateScore(players[1].cards)) << std::endl;
		std::cout << "Dealer: " << std::to_string(CalculateScore(players[0].cards)) << std::endl;

		std::string input = "";
		int numInput = 0;

		std::cout << DetermineWinner(players[0],players[1]);

		std::cout << "\n";

		std::cout << "Money: " << "\n";
		std::cout << "Dealer: " << std::to_string(players[0].money) << "\n";
		std::cout << "Player: " << std::to_string(players[1].money) << "\n";

		std::cout << "Play again (0): ";
		std::cin >> input;

		std::cout << std::endl;

		numInput = std::stoi(input);

		switch (numInput)
		{
		case 0:
			std::cout << "Place Bets Now";

			std::cout << "\n";

			PlaceBet(players[0].bet, players[0].money);
			PlaceBet(players[1].bet, players[1].money);
			break;
		default:
			inGame = false;
			break;
		}

		players[0].cards.clear();
		players[1].cards.clear();
	}

	system("pause");
	return 0;
}