#include "stdafx.h"
#include "ReverseMove.h"


ReverseMove::ReverseMove(int64_t newx, int depth, bool isItNowPlayerOne, int newWinningSetAmount, int8_t newNormalPos)
{
	binaryPos = newx;
	computationDepth = depth;
	isPlayerOne = isItNowPlayerOne;
	data = newWinningSetAmount;
	normalPos = newNormalPos;
}

ReverseMove::ReverseMove()
{
}

// sets the location x,y back to N

// the location of the current move is set back to N
void ReverseMove::reverse(Board& gameBoard)
{
	gameBoard.removeMove(binaryPos,isPlayerOne);
	gameBoard.freePositions.push_back({ binaryPos,normalPos }); // If a move is reverted its positions is free again

	// revert Heuristic
	gameBoard.updateHeuristicField(isPlayerOne,true,data, binaryPos,normalPos);
}

ReverseMove::~ReverseMove()
{
}
