#pragma once
#include "Board.h"

struct player
{
	friend class Game;
private:
	CellColour mColour = CellColour::NONE;
	bool mAIToMove = false;
};

//other test comm

class Game
{
public:
	Game();
	Game(const Game& origin);
	Game& operator= (const Game& origin);

public:
	void start();

protected:
	void setPrefs();
	void whoseWin();
	void moveProcessing();
	bool isListHasRedPath(const std::list<int>& i);
	bool isListHasBluePath(const std::list<int>& i);
	void tryGroupNodes(Node currNode, Node comparableNode, std::list<std::list<int>>* colourContainer);
	bool isConnected(const Node& currNode, const Node& comparableNode);
	bool isMoveLegal(Node node);
	void switchPlayers();

private:
	int mTurn = 1;
	bool mIsAIEnabled = false;
	char mMode;
	Board mBoard;
	player mMovalPlayer;
	CellColour mWinner = CellColour::NONE;
	bool isSimulation = false;
};
