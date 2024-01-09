#include <list>
#include <random> // have <algorithm> and <cmath>
#include <ctime>
#include "Game.h"

Game::Game()
{
	mMovalPlayer.mColour = CellColour::RED;
}

Game::Game(const Game& origin)
{
	mTurn = origin.mTurn;
	mIsAIEnabled = origin.mIsAIEnabled;
	mMode = origin.mMode;
	mBoard = origin.mBoard;
	mMovalPlayer = origin.mMovalPlayer;
	mWinner = origin.mWinner;
}

Game& Game::operator=(const Game& origin)
{
	mTurn = origin.mTurn;
	mIsAIEnabled = origin.mIsAIEnabled;
	mMode = origin.mMode;
	mBoard = origin.mBoard;
	mMovalPlayer = origin.mMovalPlayer;
	mWinner = origin.mWinner;
	return *this;
}

void Game::start()
{
	setPrefs();
	std::cout << " Game has started.\nMoves are inputed in \"raw column\" format, like \"1 2\"." << std::endl << std::endl;
	while (mWinner == CellColour::NONE)
	{
		moveProcessing();
	}
	std::cout << mWinner << " is won!" << std::endl;
}

void Game::setPrefs()
{
	// set board size
	int boardSize;
	std::cout << "Please, choose a size of your board between 4 and 20:" << std::endl;
	while (!(std::cin >> boardSize) || (boardSize < 4 || boardSize > 20))
	{
		std::cout << "Wrong input. Try again with an adequate integer." << std::endl;
	}

	// choose game mode
	std::cout << "Choose game mode:\nWith computer [c] | With other Player [p]" << std::endl;
	while (!(std::cin >> mMode) || (mMode != 'c' && mMode != 'p'))
	{
		std::cout << mMode << " is wrong input. Try again with an adequate game mode." << std::endl;
	}

	switch (mMode)
	{
	case 'c':
	case 'C':
		mIsAIEnabled = true;
		std::cout << "Choose your colour (Red 1st to move):\nRed Player [r] | Blue Player [b]" << std::endl;
		char order;
		while (!(std::cin >> order) || (order != 'r' && order != 'b'))
		{
			std::cout << order << " is wrong input. Try again with an adequate player variant." << std::endl;
		}
		if (order == 'b' || order == 'B')
		{
			mMovalPlayer.mAIToMove = true;
		}
		std::cout << "Starting Hex game versus computer." << std::endl;
		break;
	case 'p':
	case 'P':
		std::cout << "Starting default Hex PvP." << std::endl;
		break;
	default:
		break;
	}

	Board board1(boardSize);
	mBoard = board1;
	mBoard.print();
}

void Game::whoseWin()
{
	// group all nodes
	// path exist iff any list >= mSize
	for (std::list<int> i : mBoard.redNodesEncrypted)
	{
		if (i.size() >= mBoard.mSide && isListHasRedPath(i))
		{
			mWinner = CellColour::RED;
		}
	}

	for (std::list<int> i : mBoard.blueNodesEncrypted)
	{
		if (i.size() >= mBoard.mSide && isListHasBluePath(i))
		{
			mWinner = CellColour::BLUE;
		}
	}
}

void Game::moveProcessing()
{
	// if AI moves first it always hit the center
	if (mMovalPlayer.mAIToMove && mTurn == 1)
	{
		int iHexVal = trunc(mBoard.mSide / 2);
		int jHexVal = trunc(mBoard.mSide / 2);
		mBoard.mBoardColouring[iHexVal][jHexVal] = mMovalPlayer.mColour;
		Node currNode{ iHexVal, jHexVal };
		mBoard.redNodesEncrypted.push_back({ mBoard.nodeEncrypt(currNode) });
		std::cout << "AI commited the " << iHexVal + 1 << ' ' << jHexVal + 1 << std::endl;
	}
	else if (mMovalPlayer.mAIToMove)
	{
		Game* pSimulation = this;
		Game simulation;
		simulation.isSimulation = true;
		srand(time(0));
		int numberOfSimulations = 5000;
		CellColour colourOfAI = mMovalPlayer.mColour;
		std::list<int> winnableMoveList;
		//simulation move processing
		for (int n = 0; n < numberOfSimulations; n++)
		{
			simulation = *pSimulation;
			int iSimHexVal = -1;
			int jSimHexVal = -1;
			// get a pre-random move to remember 1st of those
			while (!simulation.isMoveLegal({ iSimHexVal, jSimHexVal }))
			{
				iSimHexVal = rand() % mBoard.mSide;
				jSimHexVal = rand() % mBoard.mSide;
			}
			Node firstNodeOfSimulation{ iSimHexVal, jSimHexVal };
			while (simulation.mWinner == CellColour::NONE)
			{
				while (!simulation.isMoveLegal({ iSimHexVal, jSimHexVal }))
				{
					iSimHexVal = rand() % mBoard.mSide;
					jSimHexVal = rand() % mBoard.mSide;
				}
				// record into 'simulation.mBoardColouring'
				simulation.mBoard.mBoardColouring[iSimHexVal][jSimHexVal] = simulation.mMovalPlayer.mColour;
				std::list<std::list<int>>* pColourGroup = nullptr;

				// find the container where to put the node
				switch (simulation.mMovalPlayer.mColour)
				{
				case CellColour::RED:
					pColourGroup = &simulation.mBoard.redNodesEncrypted;
					break;
				case CellColour::BLUE:
					pColourGroup = &simulation.mBoard.blueNodesEncrypted;
					break;
				default:
					std::cout << "SimulationLog. It isn't suppose to happen, but 'mMovalPlayer.mColour' is neither RED or BLUE" << std::endl;
					break;
				}

				Node currNode{ iSimHexVal, jSimHexVal };
				(*pColourGroup).push_back({ simulation.mBoard.nodeEncrypt(currNode) });
				(*pColourGroup).sort();

				// grouping hexes in routes
				// top cell
				simulation.tryGroupNodes(currNode, { iSimHexVal - 1, jSimHexVal }, pColourGroup);
				// top-right cell
				simulation.tryGroupNodes(currNode, { iSimHexVal - 1, jSimHexVal + 1 }, pColourGroup);
				// left cell
				simulation.tryGroupNodes(currNode, { iSimHexVal, jSimHexVal - 1 }, pColourGroup);
				// right cell
				simulation.tryGroupNodes(currNode, { iSimHexVal, jSimHexVal + 1 }, pColourGroup);
				// bottom-left cell
				simulation.tryGroupNodes(currNode, { iSimHexVal + 1, jSimHexVal - 1 }, pColourGroup);
				// bottom cell
				simulation.tryGroupNodes(currNode, { iSimHexVal + 1, jSimHexVal }, pColourGroup);

				simulation.whoseWin();
				if (simulation.mWinner == CellColour::NONE)
				{
					simulation.switchPlayers();
				}
			}
			if (simulation.mWinner == colourOfAI)
			{
				winnableMoveList.push_back(mBoard.nodeEncrypt(firstNodeOfSimulation));
			}
		}

		//find the move with most winnings
		int mostPlausibleMove = -1;
		int repetitionMax = 0;
		winnableMoveList.sort();
		for (int k : winnableMoveList)
		{
			int currRepetition = std::count(winnableMoveList.cbegin(), winnableMoveList.cend(), k);
			if (currRepetition > repetitionMax)
			{
				mostPlausibleMove = k;
				repetitionMax = currRepetition;
			}
		}

		// AI commit the move
		int iHexVal = mBoard.nodeDecrypt(mostPlausibleMove).iNodeVal;
		int jHexVal = mBoard.nodeDecrypt(mostPlausibleMove).jNodeVal;
		std::cout << "AI commited the " << iHexVal + 1 << ' ' << jHexVal + 1 << std::endl;
		// record into 'mBoardColouring'
		mBoard.mBoardColouring[iHexVal][jHexVal] = mMovalPlayer.mColour;

		std::list<std::list<int>>* pColourGroup = nullptr;

		// find the container where to put the node
		switch (mMovalPlayer.mColour)
		{
		case CellColour::RED:
			pColourGroup = &mBoard.redNodesEncrypted;
			break;
		case CellColour::BLUE:
			pColourGroup = &mBoard.blueNodesEncrypted;
			break;
		default:
			std::cout << "AILog. It isn't suppose to happen, but 'mMovalPlayer.mColour' is neither RED or BLUE" << std::endl;
			break;
		}

		Node currNode{ iHexVal, jHexVal };
		(*pColourGroup).push_back({ mBoard.nodeEncrypt(currNode) });
		(*pColourGroup).sort();

		// grouping hexes in routes
		// top cell
		tryGroupNodes(currNode, { iHexVal - 1, jHexVal }, pColourGroup);
		// top-right cell
		tryGroupNodes(currNode, { iHexVal - 1, jHexVal + 1 }, pColourGroup);
		// left cell
		tryGroupNodes(currNode, { iHexVal, jHexVal - 1 }, pColourGroup);
		// right cell
		tryGroupNodes(currNode, { iHexVal, jHexVal + 1 }, pColourGroup);
		// bottom-left cell
		tryGroupNodes(currNode, { iHexVal + 1, jHexVal - 1 }, pColourGroup);
		// bottom cell
		tryGroupNodes(currNode, { iHexVal + 1, jHexVal }, pColourGroup);

	}
	else
	{
		int iHexVal, jHexVal;
		std::cout << "Make a move " << mMovalPlayer.mColour << ":" << std::endl;

		while (!(std::cin >> iHexVal >> jHexVal) || !isMoveLegal({ iHexVal - 1, jHexVal - 1 }));

		iHexVal--;
		jHexVal--;

		// record into 'mBoardColouring'
		mBoard.mBoardColouring[iHexVal][jHexVal] = mMovalPlayer.mColour;

		std::list<std::list<int>>* pColourGroup = nullptr;

		// find the container where to put the node
		switch (mMovalPlayer.mColour)
		{
		case CellColour::RED:
			pColourGroup = &mBoard.redNodesEncrypted;
			break;
		case CellColour::BLUE:
			pColourGroup = &mBoard.blueNodesEncrypted;
			break;
		default:
			std::cout << "It isn't suppose to happen, but 'mMovalPlayer.mColour' is neither RED or BLUE" << std::endl;
			break;
		}

		Node currNode{ iHexVal, jHexVal };
		(*pColourGroup).push_back({ mBoard.nodeEncrypt(currNode) });
		(*pColourGroup).sort();

		// grouping hexes in routes
		// top cell
		tryGroupNodes(currNode, { iHexVal - 1, jHexVal }, pColourGroup);
		// top-right cell
		tryGroupNodes(currNode, { iHexVal - 1, jHexVal + 1 }, pColourGroup);
		// left cell
		tryGroupNodes(currNode, { iHexVal, jHexVal - 1 }, pColourGroup);
		// right cell
		tryGroupNodes(currNode, { iHexVal, jHexVal + 1 }, pColourGroup);
		// bottom-left cell
		tryGroupNodes(currNode, { iHexVal + 1, jHexVal - 1 }, pColourGroup);
		// bottom cell
		tryGroupNodes(currNode, { iHexVal + 1, jHexVal }, pColourGroup);
	}

	switchPlayers();
	mBoard.print();
	whoseWin();
	mTurn++;
}

bool Game::isListHasRedPath(const std::list<int>& i)
{
	for (int n1 : i)
	{
		if (mBoard.nodeDecrypt(n1).iNodeVal == 0)
		{
			for (int n2 : i)
			{
				if (mBoard.nodeDecrypt(n2).iNodeVal == mBoard.mSide - 1)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Game::isListHasBluePath(const std::list<int>& i)
{
	for (int n1 : i)
	{
		if (mBoard.nodeDecrypt(n1).jNodeVal == 0)
		{
			for (int n2 : i)
			{
				if (mBoard.nodeDecrypt(n2).jNodeVal == mBoard.mSide - 1)
				{
					return true;
				}
			}
		}
	}

	return false;
}

void Game::tryGroupNodes(Node currNode, Node comparableNode, std::list<std::list<int>>* pColourContainer)
{
	std::list<std::list<int>>::iterator itCurrNodeGroup;
	std::list<std::list<int>>::iterator itComparableNodeGroup;

	if (((comparableNode.iNodeVal >= 0 && comparableNode.iNodeVal <= mBoard.mSide - 1) &&
		(comparableNode.jNodeVal >= 0 && comparableNode.jNodeVal <= mBoard.mSide - 1)) &&
		isConnected(currNode, comparableNode) &&
		mBoard.mBoardColouring[currNode.iNodeVal][currNode.jNodeVal] == mBoard.mBoardColouring[comparableNode.iNodeVal][comparableNode.jNodeVal])
	{
		for (std::list<std::list<int>>::iterator it = (*pColourContainer).begin(); it != (*pColourContainer).end(); it++)
		{
			if (std::find((*it).begin(), (*it).end(), mBoard.nodeEncrypt(currNode)) != (*it).end())
			{
				itCurrNodeGroup = it;
			}
			if (std::find((*it).begin(), (*it).end(), mBoard.nodeEncrypt(comparableNode)) != (*it).end())
			{
				itComparableNodeGroup = it;
			}
		}
		if (itCurrNodeGroup != itComparableNodeGroup)
		{
			(*itComparableNodeGroup).merge(*itCurrNodeGroup);
			(*pColourContainer).erase(itCurrNodeGroup);
		}
	}
}

bool Game::isConnected(const Node& currNode, const Node& comparableNode)
{
	if (mBoard.mGridBody[mBoard.nodeEncrypt(comparableNode)][mBoard.nodeEncrypt(currNode)] == 1 ||
		mBoard.mGridBody[mBoard.nodeEncrypt(currNode)][mBoard.nodeEncrypt(comparableNode)] == 1)
	{
		return true;
	}

	return false;
}

bool Game::isMoveLegal(Node node)
{
	if ((node.iNodeVal < 0 || node.iNodeVal > mBoard.mSide - 1) || (node.jNodeVal < 0 || node.jNodeVal > mBoard.mSide - 1))
	{
		if (isSimulation == false)
		{
			std::cout << "Inputed cell are out of the board size. Try again, " << mMovalPlayer.mColour << ":" << std::endl;
		}

		return false;
	}
	switch (mBoard.mBoardColouring[node.iNodeVal][node.jNodeVal])
	{
	case CellColour::NONE:

		return true;

	case CellColour::RED:
		if (isSimulation == false)
		{
			if (mMovalPlayer.mColour == CellColour::RED)
			{
				std::cout << "You're already occupied this tile, Red. Try again: " << std::endl;
			}
			else
			{
				std::cout << "Your Red oponent already occupied this tile. Try again, Blue: " << std::endl;
			}
		}

		return false;

	case CellColour::BLUE:
		if (isSimulation == false)
		{
			if (mMovalPlayer.mColour == CellColour::BLUE)
			{
				std::cout << "You're already occupied this tile, Blue. Try again: " << std::endl;
			}
			else
			{
				std::cout << "Your Blue oponent already occupied this tile. Try again, Red: " << std::endl;
			}
		}

		return false;

	default:
		throw std::logic_error("some weird shit in isMoveLegal()\nReport for debugging.");
	}
}

void Game::switchPlayers()
{
	switch (this->mMovalPlayer.mColour)
	{
	case CellColour::RED:
		this->mMovalPlayer.mColour = CellColour::BLUE;
		break;
	case CellColour::BLUE:
		this->mMovalPlayer.mColour = CellColour::RED;
		break;
	case CellColour::NONE:
		throw std::logic_error("can't be switched; wtf r u doin?");
	default:
		break;
	}
	if (this->mIsAIEnabled)
	{
		this->mMovalPlayer.mAIToMove = (this->mMovalPlayer.mAIToMove) ? false : true;
	}
}
