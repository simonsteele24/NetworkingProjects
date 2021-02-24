#pragma once

#include <vector>
#include <string>

struct Player
{
	std::string playerName;
	std::vector<int> cards;
	float money = 0, bet = 0;
	bool isStaying = false;

	const int GetScore() 
	{
		int finalScore = 0;
		for (int i = 0; i < cards.size(); i++) 
		{
			finalScore += cards[i];
		}
		return finalScore;
	}
};

class BlackjackGamemanager
{
public:
	BlackjackGamemanager();
	~BlackjackGamemanager();

	void AddInPlayer(const std::string & _playerName);
	void RemovePlayer(const std::string& _playername);
	void InitializeHand();
	void DeInitializeGame();
	void HitPlayer();
	void StayPlayer();
	const bool CheckForEndOfGame();
	std::string DeclareWinner();

	int GetPlayerCount() { return static_cast<int>(players.size()); }
	int PickRandomCard();
	const bool CheckIfGameHasBeenInitialized() { return gameHasBeenInitialized; }
	const std::string GetNextPlayer();
	std::string PrintStatsToPlayer();
	std::vector<std::string> GetPlayerNames();
	std::string DisplayCards(const std::vector<int>& cards);

private:

	void RunDealersTurn();

	std::vector<Player> players;
	Player dealer;
	int currentPlayerIndex = 0;
	bool gameHasBeenInitialized = false;
};