#include "stdafx.h"
#include "PNSTranspositionTable.h"

PNSTranspositionTable::PNSTranspositionTable(int boardSize)
{
	tableSize = boardSize;
	transpositionTable.reserve(boardSize);
	resultOfLastLookUp = false; // SHould be changed before first read
}
PNSTableElement::PNSTableElement(unsigned short newDepth, bool newResult, int64_t playerOneBitBoard, int64_t playerTwoBitBoard)
{
	playerOneBoard = playerOneBitBoard;
	playerTwoBoard = playerTwoBitBoard;
	depth = newDepth;
	result = newResult;
}

void PNSTranspositionTable::insert(bool result, int64_t player1, int64_t player2, int depth, PNSData& data)
{
	pair<int64_t, int64_t> p = getLowestKey(player1, player2, data);//make_pair(player1, player2); //
	int64_t newHash = makeBoardsToHash(p.first, p.second);
	// if two values have the same hash the one with the smaller depth is saved
	std::unordered_map<int64_t, PNSTableElement >::const_iterator element = transpositionTable.find(newHash);
	if (element == transpositionTable.end()) // If there is no hash in thies field we just add the new one
	{
		transpositionTable.insert({ newHash,PNSTableElement(depth,result,p.first,p.second) });
	}
	else
	{
		if (p.first == element->second.playerOneBoard && p.second == element->second.playerTwoBoard) // if both elements are the same nothing happens
		{
			return;
		}
		else
		//	if (depth <= element->second.depth) // If there are two hashes there, we check which one is more valluable, because of the depth. 
			{
				//cout << "collision" << endl;
				transpositionTable.erase(newHash);
				transpositionTable.insert({ newHash,PNSTableElement(depth,result,p.first,p.second) });
			}
	}
}

pair<int64_t, int64_t> PNSTranspositionTable::getLowestKey(int64_t player1Board, int64_t player2Board, PNSData& data)
{
	// We are looking at all possible mirroiring. Note that through multiple mirroring performed after eachother there are more possible mirroring (4/8).

	// We want to find out the smallest key of P1. If there are multiple smallest Keys we also check P2 as a tie breaker. If there are still equal key this means the game is symmetric and that it does not matter which one we add

	// horizontal
	int64_t smallestKeyP1 = player1Board; // the basicKey
	int64_t smallestKeyP2 = player2Board;
	int64_t horizontalMirroredKeyP1 = data.mirrorKeyHorizontal(player1Board);
	int64_t horizontalMirroredKeyP2 = data.mirrorKeyHorizontal(player2Board);

	if (smallestKeyP1 > horizontalMirroredKeyP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
	{
		smallestKeyP1 = horizontalMirroredKeyP1;
		smallestKeyP2 = horizontalMirroredKeyP2;
	}
	else if (smallestKeyP1 == horizontalMirroredKeyP1) // If the Key of P1 are equal we check the same for the P2 key
	{
		if (smallestKeyP2 > horizontalMirroredKeyP2)
		{
			// We do not need to change smallestKeyP1 as they are equal
			smallestKeyP2 = horizontalMirroredKeyP2;
		}
	}
	//vertical

	int64_t verticalMirroredKeyP1 = data.mirrorKeyVertical(player1Board);
	if (smallestKeyP1 > verticalMirroredKeyP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
	{
		int64_t verticalMirroredKeyP2 = data.mirrorKeyVertical(player2Board);
		smallestKeyP1 = verticalMirroredKeyP1;
		smallestKeyP2 = verticalMirroredKeyP2;
	}
	else if (smallestKeyP1 == verticalMirroredKeyP1) // If the Key of P1 are equal we check the same for the P2 key
	{
		int64_t verticalMirroredKeyP2 = data.mirrorKeyVertical(player2Board);
		if (smallestKeyP2 > verticalMirroredKeyP2)
		{
			// We do not need to change smallestKeyP1 as they are equal
			smallestKeyP2 = verticalMirroredKeyP2;
		}
	}
	// Vertical and Horizontal	
	int64_t verticalAndHorizontallyMirroredKeyP1 = data.mirrorKeyVertical(horizontalMirroredKeyP1);
	if (smallestKeyP1 > verticalAndHorizontallyMirroredKeyP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
	{
		int64_t verticalAndHorizontallyMirroredKeyP2 = data.mirrorKeyVertical(horizontalMirroredKeyP2);
		smallestKeyP1 = verticalAndHorizontallyMirroredKeyP1;
		smallestKeyP2 = verticalAndHorizontallyMirroredKeyP2;
	}
	else if (smallestKeyP1 == verticalAndHorizontallyMirroredKeyP1) // If the Key of P1 are equal we check the same for the P2 key
	{
		int64_t verticalAndHorizontallyMirroredKeyP2 = data.mirrorKeyVertical(horizontalMirroredKeyP2);
		if (smallestKeyP2 > verticalAndHorizontallyMirroredKeyP2)
		{
			// We do not need to change smallestKeyP1 as they are equal
			smallestKeyP2 = verticalAndHorizontallyMirroredKeyP2;
		}
	}


	// In square games additional 4 symmetries exist (by mirroring the game diagonal)
	if (data.boardIsSquare)
	{
		// basickeymirrored Diagonal	
		int64_t basicKeyMirroredDiagonalP1 = data.mirrorKeyDiagonal(player1Board);
		int64_t basicKeyMirroredDiagonalP2 = data.mirrorKeyDiagonal(player2Board);

		if (smallestKeyP1 > basicKeyMirroredDiagonalP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
		{
			smallestKeyP1 = basicKeyMirroredDiagonalP1;
			smallestKeyP2 = basicKeyMirroredDiagonalP2;
		}
		else if (smallestKeyP1 == basicKeyMirroredDiagonalP1) // If the Key of P1 are equal we check the same for the P2 key
		{
			if (smallestKeyP2 > basicKeyMirroredDiagonalP2)
			{
				// We do not need to change smallestKeyP1 as they are equal
				smallestKeyP2 = basicKeyMirroredDiagonalP2;
			}
		}
		// diagonalmirrored Vertical
		int64_t diagonalKeyMirroredVerticalP1 = data.mirrorKeyVertical(basicKeyMirroredDiagonalP1);
		int64_t diagonalKeyMirroredVerticalP2 = data.mirrorKeyVertical(basicKeyMirroredDiagonalP2);
		if (smallestKeyP1 > diagonalKeyMirroredVerticalP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
		{
			smallestKeyP1 = diagonalKeyMirroredVerticalP1;
			smallestKeyP2 = diagonalKeyMirroredVerticalP2;
		}
		else if (smallestKeyP1 == diagonalKeyMirroredVerticalP1) // If the Key of P1 are equal we check the same for the P2 key
		{
			if (smallestKeyP2 > diagonalKeyMirroredVerticalP2)
			{
				// We do not need to change smallestKeyP1 as they are equal
				smallestKeyP2 = diagonalKeyMirroredVerticalP2;
			}
		}

		// Diagonal mirrored Horizontal
		int64_t diagonalKeyMirroredHorizontalP1 = data.mirrorKeyHorizontal(basicKeyMirroredDiagonalP1);
		if (smallestKeyP1 > diagonalKeyMirroredHorizontalP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
		{
			int64_t diagonalKeyMirroredHorizontalP2 = data.mirrorKeyHorizontal(basicKeyMirroredDiagonalP2);
			smallestKeyP1 = diagonalKeyMirroredHorizontalP1;
			smallestKeyP2 = diagonalKeyMirroredHorizontalP2;
		}
		else if (smallestKeyP1 == diagonalKeyMirroredHorizontalP1) // If the Key of P1 are equal we check the same for the P2 key
		{
			int64_t diagonalKeyMirroredHorizontalP2 = data.mirrorKeyHorizontal(basicKeyMirroredDiagonalP2);
			if (smallestKeyP2 > diagonalKeyMirroredHorizontalP2)
			{
				// We do not need to change smallestKeyP1 as they are equal
				smallestKeyP2 = diagonalKeyMirroredHorizontalP2;
			}
		}

		// vertical, horizontal and diagonal mirrored basicKey
		int64_t verticalAndHorizontallyKeyMirreredDiagonalP1 = data.mirrorKeyHorizontal(diagonalKeyMirroredVerticalP1);

		if (smallestKeyP1 > verticalAndHorizontallyKeyMirreredDiagonalP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
		{
			int64_t verticalAndHorizontallyKeyMirreredDiagonalP2 = data.mirrorKeyHorizontal(diagonalKeyMirroredVerticalP2);
			smallestKeyP1 = verticalAndHorizontallyKeyMirreredDiagonalP1;
			smallestKeyP2 = verticalAndHorizontallyKeyMirreredDiagonalP2;
		}
		else if (smallestKeyP1 == verticalAndHorizontallyKeyMirreredDiagonalP1) // If the Key of P1 are equal we check the same for the P2 key
		{
			int64_t verticalAndHorizontallyKeyMirreredDiagonalP2 = data.mirrorKeyHorizontal(diagonalKeyMirroredVerticalP2);
			if (smallestKeyP2 > verticalAndHorizontallyKeyMirreredDiagonalP2)
			{
				// We do not need to change smallestKeyP1 as they are equal
				smallestKeyP2 = verticalAndHorizontallyKeyMirreredDiagonalP2;
			}
		}

	}
	return make_pair(smallestKeyP1, smallestKeyP2);
}

bool PNSTranspositionTable::find(int64_t player1, int64_t player2, PNSData& data)
{
	pair<int64_t, int64_t> p =  getLowestKey(player1, player2, data); //make_pair(player1, player2);//
	int64_t newHash = makeBoardsToHash(p.first, p.second);
	std::unordered_map<int64_t, PNSTableElement >::const_iterator element = transpositionTable.find(newHash);
	if (element != transpositionTable.end()) // If no element exists it can not be found
	{
		if (element->second.playerOneBoard == p.first && element->second.playerTwoBoard == p.second) // If an element exists if has to be checked if it is the same element
		{
			resultOfLastLookUp = element->second.result;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

int64_t PNSTranspositionTable::makeBoardsToHash(int64_t player1, int64_t player2)
{
	return(player1 << 32 | player1 >> 32) ^ player2% 178956970;
}

