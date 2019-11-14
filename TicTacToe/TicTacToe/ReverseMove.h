#pragma once

#include "Board.h"
#include "Heuristic.h"
#include <stack>

// This small class represents the possibility to revert one specific move.

class ReverseMove
{
public:
	ReverseMove(int64_t x, int depth, bool isPlayerOne, int newWinningSetAmount, int8_t newNormalPos); 
	ReverseMove();
	void reverse(Board& gameBoard);
	~ReverseMove();
	 int computationDepth; // The depth at which this move was played
	 int64_t binaryPos; // The position of the move in binary
	 int8_t normalPos; // The position of the move in normal cordinates
	 bool isPlayerOne; // the player which move will be reverted

	 // Needed for heuristic
	 int data; // the winning setvalues of the move which was reverted
};

