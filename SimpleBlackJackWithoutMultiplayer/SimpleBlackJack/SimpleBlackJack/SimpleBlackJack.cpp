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
	std::vector<Player> players;
	players.push_back(Player());
	players.push_back(Player());

	bool inGame = true;

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

		std::cout << "Play again (0): ";
		std::cin >> input;

		std::cout << std::endl;

		numInput = std::stoi(input);

		switch (numInput)
		{
		case 0:
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