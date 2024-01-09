#pragma once

#include <iostream>
#include <iomanip>
#include <list>

// 0 = Black 8 = Gray
// 1 = Blue 9 = Light Blue
// 2 = Green a = Light Green
// 3 = Aqua b = Light Aqua
// 4 = Red c = Light Red
// 5 = Purple d = Light Purple
// 6 = Yellow e = Light Yellow
// 7 = White f = Bright White
// colorattribute = foreground + background * 16
// 
//
//  --------------------------
// \ R - 0 - 0 - 0 - 0 - 0 - 0 \
//  \ \ / \ / \ / \ / \ / \ / \ \
//   \ 0 - 0 - 0 - 0 - 0 - 0 - 0 \
//    \ \ / \ / \ / \ / \ / \ / \ \
//     \ 0 - 0 - 0 - 0 - 0 - 0 - 0 \
//      \ \ / \ / \ / \ / \ / \ / \ \
//       \ 0 - 0 - 0 - 0 - 0 - 0 - 0 \
//        \ \ / \ / \ / \ / \ / \ / \ \
//         \ 0 - 0 - 0 - 0 - 0 - 0 - 0 \
//          \ \ / \ / \ / \ / \ / \ / \ \
//           \ 0 - 0 - 0 - 0 - 0 - 0 - 0 \
//            \ \ / \ / \ / \ / \ / \ / \ \
//             \ 0 - 0 - 0 - 0 - 0 - 0 - B \
//               --------------------------
//

enum class CellColour { NONE = 0, RED, BLUE };

inline void cellDecolour(CellColour& colour)
{
    colour = CellColour::NONE;
}

static std::ostream& operator<< (std::ostream& out, CellColour colour)
{
    switch (colour)
    {
    case CellColour::NONE:
        throw std::logic_error("Can't be NONE, but it is.");
        break;
    case CellColour::RED:
        out << "Red";
        break;
    case CellColour::BLUE:
        out << "Blue";
        break;
    default:
        break;
    }
    return out;
}

struct Node
{
    int iNodeVal = -1;
    int jNodeVal = -1;
};

static std::ostream& operator<< (std::ostream& out, Node node)
{
    out << node.iNodeVal << '-' << node.jNodeVal;
    return out;
}

class Board
{
public:
    Board();
    Board(int _mSide);
    Board(const Board& origin);
    Board& operator= (const Board& origin);
    ~Board();

public:
    void print();
    void printGridBodyGraphConnectivityMatrix();

protected:
    std::list<std::list<int>> redNodesEncrypted;
    std::list<std::list<int>> blueNodesEncrypted;

protected:
    int nodeEncrypt(Node node);
    Node nodeDecrypt(int node);

private:
    int** mGridBody = nullptr;
    CellColour** mBoardColouring = nullptr;
    int mSide = 0;
    int mNodesAmount = 0;

private:
    char* intToLiteral(int n);

public:
    friend class Game;
};
