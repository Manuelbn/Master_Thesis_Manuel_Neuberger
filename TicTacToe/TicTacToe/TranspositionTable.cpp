#include "stdafx.h"
#include "TranspositionTable.h"

TableElement::TableElement(int8_t newDepth, bool newResult, int64_t playerOneBitBoard, int64_t playerTwoBitBoard)
{
	playerOneBoard = playerOneBitBoard;
	playerTwoBoard = playerTwoBitBoard;
	depth = newDepth;
	result = newResult;
}


TranspositionTable::TranspositionTable(int size)
{
	transpositionTable.reserve(size);
}

void TranspositionTable::insert(bool result,Board& gameBoard)
{
	pair<int64_t, int64_t> p = getLowestKey(gameBoard);
	int64_t hash=gameBoard.makeBitBoardsToHash(p.first,p.second);
	// if two values have the same hash the one with the smaller depth is saved
	std::unordered_map<int64_t, TableElement >::const_iterator element=transpositionTable.find(hash);
	if (element == transpositionTable.end()) // If there is no hash in thies field we just add the new one
	{
		transpositionTable.insert({ hash,TableElement(gameBoard.boardDepth,result,p.first,p.second) });
	}
	else
	{
		//cout << "collision" << endl;
		if (p.first == element->second.playerOneBoard&& p.second ==element->second.playerTwoBoard) // if both elements are the same nothing happens
		{
			return;
		}else 
			//if (gameBoard.boardDepth <= element->second.depth) // If there are two hashes there, we check which one is more valluable, because of the depth. 
		{ 
			transpositionTable.erase(hash);
			transpositionTable.insert({ hash,TableElement(gameBoard.boardDepth,result,p.first,p.second) });
		}
	}

}

pair<int64_t,int64_t> TranspositionTable::getLowestKey(Board& gameBoard)
{
	// We look at all possible mirroiring to find the smallest one. Note that through multiple mirroring performed after eachother there are more possible mirroring (4/8).

	// We want to find out the smallest key of P1. If there are multiple smallest Keys we also check P2 as a tie breaker. If there are still equal key this means the game is symmetric and that it does not matter which one we add

	// horizontal
 	int64_t smallestKeyP1 = gameBoard.bitBoardPlayerOne; // the basicKey
	int64_t smallestKeyP2 = gameBoard.bitBoardPlayerTwo;
	int64_t horizontalMirroredKeyP1 = mirrorKeyHorizontal(gameBoard.bitBoardPlayerOne, gameBoard);
	int64_t horizontalMirroredKeyP2 = mirrorKeyHorizontal(gameBoard.bitBoardPlayerTwo, gameBoard);
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

	int64_t verticalMirroredKeyP1 = mirrorKeyVertical(gameBoard.bitBoardPlayerOne, gameBoard);
	if (smallestKeyP1 > verticalMirroredKeyP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
	{
		int64_t verticalMirroredKeyP2 = mirrorKeyVertical(gameBoard.bitBoardPlayerTwo, gameBoard);
		smallestKeyP1 = verticalMirroredKeyP1;
		smallestKeyP2 = verticalMirroredKeyP2;
	}
	else if (smallestKeyP1 == verticalMirroredKeyP1) // If the Key of P1 are equal we check the same for the P2 key
	{
		int64_t verticalMirroredKeyP2 = mirrorKeyVertical(gameBoard.bitBoardPlayerTwo, gameBoard);
		if (smallestKeyP2 > verticalMirroredKeyP2)
		{
			// We do not need to change smallestKeyP1 as they are equal
			smallestKeyP2 = verticalMirroredKeyP2;
		}
	}
	// Vertical and Horizontal	
	int64_t verticalAndHorizontallyMirroredKeyP1 = mirrorKeyVertical(horizontalMirroredKeyP1, gameBoard);
	if (smallestKeyP1 > verticalAndHorizontallyMirroredKeyP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
	{
		int64_t verticalAndHorizontallyMirroredKeyP2 = mirrorKeyVertical(horizontalMirroredKeyP2, gameBoard);
		smallestKeyP1 = verticalAndHorizontallyMirroredKeyP1;
		smallestKeyP2 = verticalAndHorizontallyMirroredKeyP2;
	}
	else if (smallestKeyP1 == verticalAndHorizontallyMirroredKeyP1) // If the Key of P1 are equal we check the same for the P2 key
	{
		int64_t verticalAndHorizontallyMirroredKeyP2 = mirrorKeyVertical(horizontalMirroredKeyP2, gameBoard);
		if (smallestKeyP2 > verticalAndHorizontallyMirroredKeyP2)
		{
			// We do not need to change smallestKeyP1 as they are equal
			smallestKeyP2 = verticalAndHorizontallyMirroredKeyP2;
		}
	}


	// In square games additional 4 symmetries exist (by mirroring the game diagonal)
	if (gameBoard.isSquare)
	{
		// basickeymirrored Diagonal	
		int64_t basicKeyMirroredDiagonalP1 = mirrorKeyDiagonal(gameBoard.bitBoardPlayerOne, gameBoard);
		int64_t basicKeyMirroredDiagonalP2 = mirrorKeyDiagonal(gameBoard.bitBoardPlayerTwo, gameBoard);

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
		int64_t diagonalKeyMirroredVerticalP1 = mirrorKeyVertical(basicKeyMirroredDiagonalP1, gameBoard);
		int64_t diagonalKeyMirroredVerticalP2 = mirrorKeyVertical(basicKeyMirroredDiagonalP2, gameBoard);
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
		int64_t diagonalKeyMirroredHorizontalP1 = mirrorKeyHorizontal(basicKeyMirroredDiagonalP1, gameBoard);
		if (smallestKeyP1 > diagonalKeyMirroredHorizontalP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
		{
			int64_t diagonalKeyMirroredHorizontalP2 = mirrorKeyHorizontal(basicKeyMirroredDiagonalP2, gameBoard);
			smallestKeyP1 = diagonalKeyMirroredHorizontalP1;
			smallestKeyP2 = diagonalKeyMirroredHorizontalP2;
		}
		else if (smallestKeyP1 == diagonalKeyMirroredHorizontalP1) // If the Key of P1 are equal we check the same for the P2 key
		{
			int64_t diagonalKeyMirroredHorizontalP2 = mirrorKeyHorizontal(basicKeyMirroredDiagonalP2, gameBoard);
			if (smallestKeyP2 > diagonalKeyMirroredHorizontalP2)
			{
				// We do not need to change smallestKeyP1 as they are equal
				smallestKeyP2 = diagonalKeyMirroredHorizontalP2;
			}
		}

		// vertical, horizontal and diagonal mirrored basicKey
		int64_t verticalAndHorizontallyKeyMirreredDiagonalP1 = mirrorKeyHorizontal(diagonalKeyMirroredVerticalP1, gameBoard);

		if (smallestKeyP1 > verticalAndHorizontallyKeyMirreredDiagonalP1) // if the old smallest key is bigger than the old one we change it. The smallestKeyP2 is not the smallest key of all P2 values but the smallest P2 value of all smallest P1 values
		{
			int64_t verticalAndHorizontallyKeyMirreredDiagonalP2 = mirrorKeyHorizontal(diagonalKeyMirroredVerticalP2, gameBoard);
			smallestKeyP1 = verticalAndHorizontallyKeyMirreredDiagonalP1;
			smallestKeyP2 = verticalAndHorizontallyKeyMirreredDiagonalP2;
		}
		else if (smallestKeyP1 == verticalAndHorizontallyKeyMirreredDiagonalP1) // If the Key of P1 are equal we check the same for the P2 key
		{
			int64_t verticalAndHorizontallyKeyMirreredDiagonalP2 = mirrorKeyHorizontal(diagonalKeyMirroredVerticalP2, gameBoard);
			if (smallestKeyP2 > verticalAndHorizontallyKeyMirreredDiagonalP2)
			{
				// We do not need to change smallestKeyP1 as they are equal
				smallestKeyP2 = verticalAndHorizontallyKeyMirreredDiagonalP2;
			}
		}

	}
	return make_pair(smallestKeyP1,smallestKeyP2);
}

bool TranspositionTable::find(Board& gameBoard)
{
	// Searches the table for the current state of the board
	pair<int64_t, int64_t> p = getLowestKey(gameBoard); //
	int64_t newHash =gameBoard.makeBitBoardsToHash(p.first,p.second);
	std::unordered_map<int64_t, TableElement >::const_iterator element = transpositionTable.find(newHash);
	if (element != transpositionTable.end()) // If no element exists it can not be found
	{
		if (element->second.playerOneBoard ==p.first&&element->second.playerTwoBoard==p.second) // If an element exists if has to be checked if it is the same element
		{
			gameBoard.resultOfTableLookUp = element->second.result;
			gameBoard.transpositionTableHits++;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}



int64_t TranspositionTable::mirrorKeyHorizontal(int64_t playerKey, Board& gameBoard)
{
	// To mirror the board in the horizontal we basically isolate each row and shift it to the new position.
	int nhalf = gameBoard.getN() / 2;
	int64_t horizonalMirroringKey = 0;
	int n = gameBoard.getN();
	for (int i = 0; i < n; i++) // N is also the size of the relevant bitmasks
	{
		if (i < nhalf)
		{
			horizonalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForHorizontalMirroring[i]) << (n - 2 * i - 1) * (gameBoard.getM() + 1);
		}
		else if (i > nhalf || (i >= nhalf && n % 2 == 0)) // In the case of N is odd the middle row is not changed
		{
			horizonalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForHorizontalMirroring[i]) >> (2 * i - n+1) * (gameBoard.getM() + 1);
		}
		else
			if (i== nhalf&&n %2==1)
			{
				//if the gameboard s is odd, the middle row has to be added as well. The row is unchanged
				horizonalMirroringKey |= playerKey & gameBoard.mirroringBitMasks.bitMasksForHorizontalMirroring[i];
			}
	}
	return horizonalMirroringKey;
}

int64_t TranspositionTable::mirrorKeyVertical(int64_t playerKey, Board& gameBoard)
{
	// To mirror the board in the vertical we basically isolate each column and shift it to the new position.
	int64_t verticalMirroringKey = 0;
	int mhalf = gameBoard.getM() / 2;
	for (int i = 0; i < gameBoard.getM(); i++)
	{
		if (i < mhalf)
		{
			verticalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForVerticalMirroring[i]) << (gameBoard.getM()-1 - 2 * i);
		}
		else if (i > mhalf || (i >= mhalf && gameBoard.getM() % 2 == 0)) // In the case of N is odd the middle row is not changed
		{
			verticalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForVerticalMirroring[i]) >> (2 * i - gameBoard.getM()+1);
		}
		else
			if (i == mhalf && gameBoard.getM() % 2==1)
			{
				//if the gameboard m is odd, the middle colum has to be added as well. The row is unchanged
				verticalMirroringKey |= playerKey & gameBoard.mirroringBitMasks.bitMasksForVerticalMirroring[i];
			}
	}
	return verticalMirroringKey;
}

int64_t TranspositionTable::mirrorKeyDiagonal(int64_t playerKey, Board& gameBoard)
{
//Although there is a diagonal and an antidiagonal we only need to compute one, as the other can be created by mirroring the diagonal

// The idea of the diagonal mirroring is as follows:
// Each element is moved depending of it distance to the diagonal. The two X below both have a distance of one to the diagonal (here we only count the horizontal/vertical distance). 
// If we now move these X by m -1 (or m if we take the dead row into account), both Xs are mirrored. 
//The O now has to be moved by 2* (m-1) [2m] to get to the correct postion. This strategie can be expanded for bigger fields very easy
//	O X -
//  X - -
//  - - -
	int64_t diagonalMirroringKey = gameBoard.mirroringBitMasks.bitMasksForDiagonalMirroringOne[0] & playerKey;
	for (int i = 1; i < gameBoard.getN(); i++)
	{
		diagonalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForDiagonalMirroringOne[i]) >> gameBoard.getM() * i;
	}
	for (int i = 1; i < gameBoard.getN(); i++)
	{
		diagonalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForDiagonalMirroringTwo[i]) << gameBoard.getM() * i;
	}
	return diagonalMirroringKey;
}
