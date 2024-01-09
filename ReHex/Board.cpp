
#include <Windows.h>
#include "Board.h"

Board::Board()
{
}

Board::Board(int _mSide)
    : mSide(_mSide), mNodesAmount(_mSide* _mSide)
{
    // allocate memory for graph
    mGridBody = new int* [mNodesAmount];
    for (int i = 0; i < mNodesAmount; i++)
    {
        mGridBody[i] = new int[mNodesAmount];
    }

    // allocate memory for colouring
    mBoardColouring = new CellColour * [mSide];
    for (int i = 0; i < mSide; i++)
    {
        mBoardColouring[i] = new CellColour[mSide];
    }

    // fill connectivity matrix
    for (int i = 0; i < mNodesAmount; i++)
    {
        for (int j = 0; j < mNodesAmount; j++)
        {
            // default case
            mGridBody[j][i] = -1;
        }
        // we're good only with upper-triangular matrix
        for (int j = 0; j < i; j++)
        {
            if ((nodeDecrypt(i).jNodeVal == nodeDecrypt(j).jNodeVal && // top connection
                nodeDecrypt(i).iNodeVal == nodeDecrypt(j).iNodeVal + 1) ||
                (nodeDecrypt(i).jNodeVal == nodeDecrypt(j).jNodeVal - 1 && // top-right
                    nodeDecrypt(i).iNodeVal == nodeDecrypt(j).iNodeVal + 1) ||
                (nodeDecrypt(i).jNodeVal == nodeDecrypt(j).jNodeVal + 1 && // left
                    nodeDecrypt(i).iNodeVal == nodeDecrypt(j).iNodeVal))
            {
                mGridBody[j][i] = 1;
            }
            // eventually all right, bottom-left and bottom will be filled
        }
    }

    // set all colours to 'NONE'
    for (int i = 0; i < mSide; i++)
    {
        for (int j = 0; j < mSide; j++)
        {
            mBoardColouring[i][j] = CellColour::NONE;
        }
    }
}

Board::Board(const Board& origin)
    : mSide(origin.mSide), mNodesAmount(origin.mNodesAmount)
{
    mGridBody = new int* [mNodesAmount];
    for (int i = 0; i < mNodesAmount; i++)
    {
        mGridBody[i] = new int[mNodesAmount];
    }

    for (int i = 0; i < mNodesAmount; i++)
    {
        for (int j = 0; j < mNodesAmount; j++)
        {
            mGridBody[i][j] = origin.mGridBody[i][j];
        }
    }

    mBoardColouring = new CellColour * [mSide];
    for (int i = 0; i < mSide; i++)
    {
        mBoardColouring[i] = new CellColour[mSide];
    }

    for (int i = 0; i < mSide; i++)
    {
        for (int j = 0; j < mSide; j++)
        {
            mBoardColouring[i][j] = origin.mBoardColouring[i][j];
        }
    }
}

Board& Board::operator= (const Board& origin)
{
    mSide = origin.mSide;
    mNodesAmount = origin.mNodesAmount;

    mGridBody = new int* [mNodesAmount];
    for (int i = 0; i < mNodesAmount; i++)
    {
        mGridBody[i] = new int[mNodesAmount];
    }

    for (int i = 0; i < mNodesAmount; i++)
    {
        for (int j = 0; j < mNodesAmount; j++)
        {
            mGridBody[i][j] = origin.mGridBody[i][j];
        }
    }

    mBoardColouring = new CellColour * [mSide];
    for (int i = 0; i < mSide; i++)
    {
        mBoardColouring[i] = new CellColour[mSide];
    }

    for (int i = 0; i < mSide; i++)
    {
        for (int j = 0; j < mSide; j++)
        {
            mBoardColouring[i][j] = origin.mBoardColouring[i][j];
        }
    }

    redNodesEncrypted = origin.redNodesEncrypted;
    blueNodesEncrypted = origin.blueNodesEncrypted;

    return *this;
}

Board::~Board()
{
    for (int i = 0; i < mSide; i++)
    {
        delete[] mBoardColouring[i];
    }
    for (int i = 0; i < mNodesAmount; i++)
    {
        delete[] mGridBody[i];
    }
    delete[] mBoardColouring;
    delete[] mGridBody;
}

void Board::print()
{
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout << ' ';
    SetConsoleTextAttribute(hConsole, 4);
    // top border
    for (int i = 0; i < mSide * 4 - 2; i++)
    {
        std::cout << (char)196;
    }
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << std::endl;

    for (int lineCounter = 0; lineCounter < 2 * mSide - 1; lineCounter++)
    {
        // spaces before an each line
        for (int k = lineCounter; k > 0; k--)
        {
            std::cout << ' ';
        }

        SetConsoleTextAttribute(hConsole, 1);
        // left border
        std::cout << "\\ ";
        SetConsoleTextAttribute(hConsole, 7);
        switch (lineCounter % 2)
        {
        case 0:
            for (int jHexVal = 0; jHexVal < mSide; jHexVal++)
            {
                int iHexVal = lineCounter / 2;
                switch (mBoardColouring[iHexVal][jHexVal])
                {
                case CellColour::NONE:
                    SetConsoleTextAttribute(hConsole, 8);
                    std::cout << "0";
                    break;
                case CellColour::RED:
                    SetConsoleTextAttribute(hConsole, 4);
                    std::cout << "R";
                    break;
                case CellColour::BLUE:
                    SetConsoleTextAttribute(hConsole, 1);
                    std::cout << "B";
                    break;
                default:
                    SetConsoleTextAttribute(hConsole, 1 + 4 * 16);
                    // for debug
                    std::cout << "X";
                    break;
                }
                SetConsoleTextAttribute(hConsole, 7);
                if (jHexVal + 1 != mSide)
                {
                    std::cout << " " << (char)196 << " ";
                }
            }
            break;
            // draws horisontal connections
        case 1:
            for (int k = 0; k < mSide - 1; k++)
            {
                std::cout << "\\ / ";
            }
            std::cout << "\\";
            break;
        default:
            // for debug
            std::cout << "Doesn't suppose to happen" << std::endl;
            break;
        }
        SetConsoleTextAttribute(hConsole, 1);
        // right border
        std::cout << " \\" << std::endl;
        SetConsoleTextAttribute(hConsole, 7);
    }

    // bottom border
    for (int i = 0; i < 2 * mSide; i++)
    {
        std::cout << ' ';
    }
    SetConsoleTextAttribute(hConsole, 4);
    for (int i = 0; i < mSide * 4 - 2; i++)
    {
        std::cout << (char)196;
    }
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << std::endl;
}

void Board::printGridBodyGraphConnectivityMatrix()
{
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    std::cout << ' ';
    for (int i = 0; i < mNodesAmount; i++)
    {
        std::cout << std::setw(2) << intToLiteral(i);
    }
    std::cout << std::endl;

    for (int i = 0; i < mNodesAmount; i++)
    {
        std::cout << intToLiteral(i);
        for (int j = 0; j < mNodesAmount; j++)
        {
            if (mGridBody[i][j] > 0)
            {
                SetConsoleTextAttribute(hConsole, 15 + 5 * 16);
                std::cout << std::setw(2) << "+";
                SetConsoleTextAttribute(hConsole, 7);
            }
            else
            {
                std::cout << std::setw(2) << "-";
            }
            if (j == mNodesAmount - 1)
            {
                std::cout << std::endl;
            }
        }
    }
}

inline char* Board::intToLiteral(int n)
{
    char* letter = new char[1];
    if (n < 26)
    {
        letter[0] = 'A' + n;
    }
    else
    {
        letter[0] = 'a' + n - 26;
    }
    letter[1] = '\0';
    return letter;
}

int Board::nodeEncrypt(Node node)
{
    return (node.iNodeVal * this->mSide + node.jNodeVal);
}

Node Board::nodeDecrypt(int node)
{
    return { (node - node % this->mSide) / this->mSide, node % this->mSide };
}
