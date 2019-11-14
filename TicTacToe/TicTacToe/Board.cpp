#include "stdafx.h"
#include "board.h"


Board::Board(int newM, int newN, int newK)
{
	boardDepth = 0;
	m = newM;
	n = newN;
	k = newK;
	turnId = 0;
	// Initalise freePositions
	int allPossiblePositionsSize = (newM + 1) * newN;
	// Fills the freePositions array
	for (int64_t i = 0; i < allPossiblePositionsSize; i++)
	{
		if (i % (m + 1) != m) // Some positions represent Empty stoper positions. These shell not be added to the possible moves
		{
			int64_t numb = 1;
			freePositions.push_back({ (numb << i),i }); // the first part of the pair is the binary position the second on is the normal one
		}
	}
	int freePositionsSize = newM * newN;
	freePositions.reserve(freePositionsSize); // Reserves memory for the vector. 
	// Is the board square
	if (m == n)
	{
		isSquare = true;
	}
	else
	{
		isSquare = false;
	}

	// The diffvalues are importend for the win check.
	initaliseDiffValues(newM, newN);

	// Relevant for transpositionTables
	resultOfTableLookUp = false; // The inital value should never be used
	// Relevant for forcedMoves
	nextMoveIsForced = false;
	
	// Importend for Symmetry
	fillBitMasks();

	// Relevant for Heuristics
	winningSetHeuristic.initializeHeuristic(m,n,k);
	winningSetHeuristic.initializeHeuristicField();
}

int Board::getM()
{
	return m;
}

int Board::getN()
{
	return n;
}

int Board::getK()
{
	return k;
}

void Board::initaliseDiffValues(int64_t m, int64_t n)
{// Basically the 
	diffVertical = m + 1;
	diffDiagonal = diffVertical + 1;
	diffAntidiagonal = m;
}

void Board::addMove(int64_t x, bool playerOnesTurn)
{
	boardDepth++;
	if (playerOnesTurn)
	{
		bitBoardPlayerOne |= x;
	}
	else
	{
		bitBoardPlayerTwo |= x;
	}
}

void Board::removeMove(int64_t x, bool isPlayerOne)
{
	boardDepth--;
	if (isPlayerOne)
	{
		bitBoardPlayerOne ^= x; // The ^ is the XOR operation. It only keeps the bits which are in one and only one of the two sequences
	}
	else
	{
		bitBoardPlayerTwo ^= x;
	}
}


bool Board::winCheck(bool playerOneCheck)
{
	// The winchecks function checks if a player has won the current board. The player for which it is checked is defined by the playerOneCheck variable
	// The idea is to apply the AND operator to the board and its the shifted k-1 versions per dimension. Each of this versions is shifted by the diffvalue 
	// The diffvalue is the distence between two positions in the bit in this dimension. by aplling the And operator all bit of a potantil line are combined.
	// The empty column on the right prevents false positives

	int64_t horizontalWin;
	int64_t verticalWin;
	int64_t diagonalWinOne;
	int64_t diagonalWinTwo;
	if (playerOneCheck)
	{
		horizontalWin = bitBoardPlayerOne;
		verticalWin = bitBoardPlayerOne;
		diagonalWinOne = bitBoardPlayerOne;
		diagonalWinTwo = bitBoardPlayerOne;
	}
	else
	{
		horizontalWin = bitBoardPlayerTwo;
		verticalWin = bitBoardPlayerTwo;
		diagonalWinOne = bitBoardPlayerTwo;
		diagonalWinTwo = bitBoardPlayerTwo;
	}
	for (int i = 0; i < k-1; i++)
	{
		horizontalWin = horizontalWin & (horizontalWin >> diffHorizontal);
		verticalWin = verticalWin & (verticalWin >> diffVertical);
		diagonalWinOne = diagonalWinOne & (diagonalWinOne >> diffDiagonal);
		diagonalWinTwo = diagonalWinTwo & (diagonalWinTwo >> diffAntidiagonal);
	}
	return horizontalWin !=0||verticalWin!=0|| diagonalWinOne!=0|| diagonalWinTwo!=0;


}
bool Board::winCheck(int64_t board)
{
	// The winchecks function checks if a player has won the current board. 
	// The idea is to apply the AND operator to the board and its the shifted k-1 versions per dimension. Each of this versions is shifted by the diffvalue 
	// The diffvalue is the distence between two positions in the bit in this dimension. by aplling the And operator all bit of a potantil line are combined.
	// The empty column on the right prevents false positives
	int64_t horizontalWin = board;
	int64_t verticalWin = board;
	int64_t diagonalWinOne = board;
	int64_t diagonalWinTwo = board;

	for (int i = 0; i < k - 1; i++)
	{
		horizontalWin = horizontalWin & (horizontalWin >> diffHorizontal);
		verticalWin = verticalWin & (verticalWin >> diffVertical);
		diagonalWinOne = diagonalWinOne & (diagonalWinOne >> diffDiagonal);
		diagonalWinTwo = diagonalWinTwo & (diagonalWinTwo >> diffAntidiagonal);
	}
	return horizontalWin != 0 || verticalWin != 0 || diagonalWinOne != 0 || diagonalWinTwo != 0;
}


int64_t Board::makeBitBoardsToHash(int64_t player1, int64_t player2)
{
	return ((player1 << 32 | player1 >> 32) ^ player2) % 178956970;
}

void Board::fillBitMasks()
{
	// HorrizontalMask
	int64_t initialHorrizontalMask =(int64_t) pow(2, m)-1;
	for (int i = 0; i < n; i++)
	{
		mirroringBitMasks.bitMasksForHorizontalMirroring[i] = initialHorrizontalMask<<i*(m+1);
	}
	// verticalMask
	int64_t initialVerticalMask = 0;
	for (int i = 0; i < n; i++)
	{
		initialVerticalMask += (int64_t)pow(2,i*(m+1));
	}

	for (int i = 0; i < m; i++)
	{
		mirroringBitMasks.bitMasksForVerticalMirroring[i] = initialVerticalMask << i;
	}
	// diaginalMask - The diagonal starts in the left bottom corner and then goes up to the right upper corner.
	// For the diagonal mirroring two arraies are used for mirroring form the left to the right and reverse (for one mirroring action we need to applie both once)
	int64_t nextMask;
	// LeftToRight
	for (int64_t i = 0; i < n; i++)
	{
		nextMask=0;
		for (int64_t j = 0; j < n-i; j++)
		{
			nextMask +=(int64_t) pow((int64_t)2, (i+j)*(m + (int64_t)1) + j);
		}
		mirroringBitMasks.bitMasksForDiagonalMirroringOne[i] = nextMask;
	}
	// RightToLeft
	for (int i = 0; i < n; i++)
	{
		nextMask = 0;
		for (int j = 0; j < n - i; j++)
		{
			nextMask += (int64_t)pow(2, (j) * (m + 1) + i+j);
		}
		mirroringBitMasks.bitMasksForDiagonalMirroringTwo[i] = nextMask;
	}




	// BitMasks for move adding
	// horizontal
	symmetryBitMasks.bitMaskForHorizontalSymmetryOnlyBottomHalf = 0;
	for (int i = 0; i < (n / 2 + n % 2); i++)
	{
		symmetryBitMasks.bitMaskForHorizontalSymmetryOnlyBottomHalf += mirroringBitMasks.bitMasksForHorizontalMirroring[i];
	}
	// vertical
	symmetryBitMasks.bitMaskForVerticalSymmetryOnlyLeftHalf = 0;
	for (int i = 0; i < (m/2+m%2); i++)
	{
		symmetryBitMasks.bitMaskForVerticalSymmetryOnlyLeftHalf += mirroringBitMasks.bitMasksForVerticalMirroring[i];
	}
	// diagonal
	symmetryBitMasks.bitMaskForDiagonalSymmetryOnlyBottomHalf = 0;
	for (int i = 0; i < 8; i++)
	{
		symmetryBitMasks.bitMaskForDiagonalSymmetryOnlyBottomHalf += mirroringBitMasks.bitMasksForDiagonalMirroringTwo[i];
	}
	// antidiagonal
	symmetryBitMasks.bitMaskForAntiDiagonalSymmetryOnlyBottomHalf =mirrorKeyVertical(symmetryBitMasks.bitMaskForDiagonalSymmetryOnlyBottomHalf,*this); // Instead of mirroring antidiagonal we can also apply the other three mirrorings

}

void Board::printGameBoard()
{
	// only needed for Debugging
	for (int i = n-1; i >= 0; i--)
	{
		for (int j = 0; j < m+1; j++)
		{
			if (j == m)
			{
				cout << " | ";
			}

			int64_t shifter =1;
			shifter = shifter << (i * (m + 1) + j);
			int numb = 0;
			if(bitBoardPlayerOne & shifter)
			{
			numb++;
			}
			if (bitBoardPlayerTwo & shifter)
			{
				numb--;
			}

			if (numb != -1)
			{
				cout << " ";
			}
			cout << numb<<" ";
		}
		cout<<endl;
	}
	cout << endl;
}

int64_t Board::mirrorKeyHorizontal(int64_t playerKey, Board& gameBoard)
{
	// To mirror the board in the horizontal we basically isolate each row and shift it to the new position.
	int nhalf = gameBoard.getN() / 2;
	int evenpart = (gameBoard.getN() + 1) % 2; // if the game is even we want to bit shift more then if it is odd
	int64_t horizonalMirroringKey = 0;
	int n = gameBoard.getN();
	for (int i = 0; i < n; i++) // N is also the size of the relevant bitmasks
	{
		if (i < nhalf)
		{
			horizonalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForHorizontalMirroring[i]) << (2 * nhalf - 2 * i - evenpart) * (gameBoard.getM() + 1);
		}
		else if (i > nhalf || (i >= nhalf && gameBoard.getN() % 2 == 0)) // In the case of N is odd the middle row is not changed
		{
			horizonalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForHorizontalMirroring[i]) >> (2 * i - 2 * nhalf + evenpart) * (gameBoard.getM() + 1);
		}
		else
			if (i == nhalf && gameBoard.getN() % 2 == 1)
			{
				//if the gameboard s is odd, the middle row has to be added as well. The row is unchanged
				horizonalMirroringKey |= playerKey & gameBoard.mirroringBitMasks.bitMasksForHorizontalMirroring[i];
			}
	}
	return horizonalMirroringKey;
}

int64_t Board::mirrorKeyVertical(int64_t playerKey, Board& gameBoard)
{
	// To mirror the board in the vertical we basically isolate each column and shift it to the new position.
	int64_t verticalMirroringKey = 0;
	int nhalf = gameBoard.getN() / 2;
	int mhalf = gameBoard.getN() / 2;
	int evenpart = (gameBoard.getN() + 1) % 2; // if the game is even we want to bit shift more then if it is odd
	for (int i = 0; i < gameBoard.getM(); i++)
	{
		if (i < mhalf) // Bottom half
		{
			verticalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForVerticalMirroring[i]) << (2 * mhalf - 2 * i - evenpart);
		}
		else if (i > mhalf || (i >= mhalf && gameBoard.getM() % 2 == 1)) // top half
		{
			verticalMirroringKey |= (playerKey & gameBoard.mirroringBitMasks.bitMasksForVerticalMirroring[i]) >> (2 * i - 2 * nhalf + evenpart);
		}
		else
			if (i == nhalf && gameBoard.getM() % 2 == 1)
			{
				//if the gameboard m is odd, the middle colum has to be added as well. The row is unchanged
				verticalMirroringKey |= playerKey & gameBoard.mirroringBitMasks.bitMasksForVerticalMirroring[i];
			}
	}
	return verticalMirroringKey; // The vertical mirroredkey
}

int64_t Board::mirrorKeyDiagonal(int64_t playerKey, Board& gameBoard)
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

// If a player sets a stone on the gameboard it needs to be updated. As we ignore Player 2 victory (strategie stealing), we only use the board with the winning sets for player 1. As player 1 does not destroy any winning sets, only the computation for P2 is really complecated
// If player one plays a stone only this position needs to set to 0 (a full position is no longer a valid option). If Player 2 places a stone all positions which shared a winning set with it need to be updated (-1).
// mPosition and nPosition are the position which has been played, player is the player who played it. isReverse should only be true while reversing the gameboard. Data is also only relevent if 
// the game is currently reverted. Data stores the number of winning set partisipations for the position which is reverted.
// The completePosition is the position of mPosition and nPosition in the bitboards.
void Board::updateHeuristicField( bool isPlayerOne, bool isReverse, int data, int64_t binaryPos, int normalPos)
{
	// If if the move is of P2 much more moves need to be included
	if (!isReverse)
	{ // If the call is a regular move, the position is set to zero.
		winningSetHeuristic.makeEntryZero(normalPos);
	}
	else
	{
		// IF the call is a reverse move, the position is set to its old value
		winningSetHeuristic.setPosition(data, normalPos);
	}
	
	if (!isPlayerOne) // if player one plays a stone only its location is changed.
	{
		// Additonally all values in the surroundings need to be updated. We do this devided by dimension (vertical, horizontal, diagonal, antidiagonal)


		// To look into the neighborhood we need the position in the m and n direction of the current move. So we have to convert them
		int mPosition = normalPos % (m + 1);
		int nPosition = normalPos / (m + 1);

		int reverseFaktor = 1;
		if (isReverse)
		{
			reverseFaktor = -1; // If the call shell reverse the game board back, the variavle is negative
		}

		// The generall parts are the same for all dimensions. 
		// In part 1, we find out how much space is into both directions (e.g. bottom and top). The distance can not be greater then k, as fields which are further away can not be affected by this change.
		// In part 2, we first check if the new area is big enoth for a single winning set, if so we iterate over the possible Length. The amount of winnigsets which are removed/added by removing/adding the current location
		// at the location depends on the distance and k. We start from one side and increase the field there by the decreaser = 1, the next field (on closer to the current location) we rase the decreaser by 1 to 2 and so on. 
		//This is done until we reach the middle or k. If we reach k first we stop incresing the decreaser. In the second half we do the same thing reversed.


		// We start with the horizontal

		// Part 1
		int spaceLeft = 0;
		for (int i = 1; mPosition - i >= 0 && i < k; i++)
		{
			if ((bitBoardPlayerTwo & (binaryPos >> i)) == 0) // as long as there is no mark of P2, the board does not end and the distance is less then k, a winning set contaning m n could go further to the left
			{
				spaceLeft++;
			}
			else
			{
				i = k; // breaks out of the loop
			}
		}
		int spaceRight = 0;
		for (int i = 1; mPosition + i < m && i < (int)k; i++)
		{
			if ((bitBoardPlayerTwo & (binaryPos << i)) == 0) // as long as there is no mark of P2, the board does not end and the distance is less then k, a winning set contaning m n could go further to the left
			{
				spaceRight++;
			}
			else
			{
				i = k; // breaks out of the loop
			}
		}

		// Part 2
		if (spaceRight + spaceLeft + 1 >= (int)k) // if the possible space is uint8_ter then k it is not possible that there was a winning set before
		{
			int possibleLength = spaceRight + spaceLeft + 1; // the space in which winning sets will be decreased (or increased if this is a reverse call)
			int halfLength = (int)ceil(possibleLength / 2.0f);// if possivleLength is odd halfLength should be rounded up

			int maxDecreaser = min(possibleLength + 1 - (int)k, (int)k); // The maximal amount by which the values can be decreased 
			int decreaser = 1; // The amout by which the next value will be decreased

			int64_t firstPositionforBitboard  = binaryPos >>spaceLeft;
			int firstPositionForHeuristic = normalPos -spaceLeft;
			for (int i = 0; i < possibleLength; i++) // now we want to decrease the values so that only the possible winning sets are represented
				// for this we iterate from the left to the right. The maximum number of winningsets a position can be part of in this dimension is k. At the borders the number increase by 1 each field.
				// e.g 1 2 3 3 3 2 1 -- example for k =3
			{
				int64_t currentLocationInBitboard = (firstPositionforBitboard << (i));
				int currentLocationForHeuristic = firstPositionForHeuristic+i;
				if (i != spaceLeft && (bitBoardPlayerOne & currentLocationInBitboard) == 0 && (bitBoardPlayerTwo & currentLocationInBitboard) == 0&&(winningSetHeuristic.getPosition(currentLocationForHeuristic)!=0||isReverse)) // If i = spaceDown we are at the original location which we have set to the correct value at the start.
				// If not so we check if the location is empty on both boards
				{
					winningSetHeuristic.decreaseWinningSetCount(currentLocationForHeuristic, decreaser * reverseFaktor);
				}
				if (halfLength > i + 1) // first half		// Basically cast i to 0 as the length is in dependent of the start position
				{
					if (decreaser < maxDecreaser) // raise the decreaser until it reaches kLength or it is over the halfLength
					{
						decreaser++;
					}
				}
				else // second half
				{
					if (possibleLength - i - 1 < maxDecreaser)
					{
						decreaser--;
					}
				}

			}
		}



		////-------------------------- vertical -> works like the horrizontal only the value jumps are different

		int spaceDown = 0;
		for (int i = 1; nPosition - i >= 0 && i < k; i++)
		{
			if ((bitBoardPlayerTwo & (binaryPos >> i * (m + 1))) == 0) // as long as there is no mark of P2, the board does not end and the distance is less then k, a winning set contaning m n could go further to the left
			{
				spaceDown++;
			}
			else
			{
				i = k; // breaks out of the loop
			}
		}
		int spaceUp = 0;
		for (int i = 1; nPosition + i < n && i < k; i++)
		{
			if ((bitBoardPlayerTwo & (binaryPos << i * (m + 1))) == 0)
			{
				spaceUp++;
			}
			else
			{
				i = k; // breaks out of the loop
			}
		}


		if (spaceDown + spaceUp + 1 >= k) // if the possible space is uint8_ter then k it is not possible that there was a winning set before
		{
			int possibleLength = spaceUp + spaceDown + 1;
			int halfLength = (int)ceil(possibleLength / 2.0f);// if possivleLength is odd halfLength should be rounded up
			int maxDecreaser = min(possibleLength + 1 - k, k);
			int decreaser = 1;

			int64_t firstPositionForBitboard= binaryPos >>spaceDown*(m+1);
			int firstPositionForHeuristic = normalPos - spaceDown*(m+1);
			for (int i = 0; i < possibleLength; i++) // now we want to decrease the values so that only the possible winning sets are represented
			{
				int64_t currentLocationInBitboard = (firstPositionForBitboard << i  * (m + 1));
				int currentLocatinOnHeuristic = firstPositionForHeuristic + i * (m + 1);
				if (i != spaceDown && (bitBoardPlayerOne & currentLocationInBitboard) == 0 && (bitBoardPlayerTwo & currentLocationInBitboard) == 0 && (winningSetHeuristic.getPosition(currentLocatinOnHeuristic) != 0 || isReverse))
				{
					winningSetHeuristic.decreaseWinningSetCount(currentLocatinOnHeuristic, decreaser * reverseFaktor);
				}
				if (halfLength > i + 1) // first half		// Basically cast i to 0 as the length is in dependent of the start position
				{
					if (decreaser < maxDecreaser) // raise the decreaser until it reaches kLength or it is over the halfLength
					{
						decreaser++;
					}
				}
				else // second half
				{
					if (possibleLength - i - 1 < maxDecreaser)
					{
						decreaser--;
					}
				}

			}
		}

		/////// --------------------------- DIAGONAL  


		int spaceBottomLeft = 0;

		for (int i = 1; nPosition - i >= 0 && mPosition - i >= 0 && i < k; i++)
		{
			if ((bitBoardPlayerTwo & (binaryPos >> (i * (m + 1) + i))) == 0) // as long as there is no mark of P2, the board does not end and the distance is less then k, a winning set contaning m n could go further to the left
			{
				spaceBottomLeft++;
			}
			else
			{
				i = k; // breaks out of the loop 
			}
		}
		int SpaceUpRight = 0;
		for (int i = 1; nPosition + i < n && mPosition + i < m && i < k; i++)
		{
			if ((bitBoardPlayerTwo & (binaryPos << (i * (m + 1) + i))) == 0) // as long as there is no mark of P2, the board does not end and the distance is less then k, a winning set contaning m n could go further to the left
			{
				SpaceUpRight++;
			}
			else
			{
				i = k; // breaks out of the loop. mLength would have the same effekt here
			}
		}

		if (spaceBottomLeft + SpaceUpRight + 1 >= k) // if the possible space is uint8_ter then k it is not possible that there was a winning set before
		{
			int possibleLength = spaceBottomLeft + SpaceUpRight + 1;
			int halfLength = (int)ceil(possibleLength / 2.0f);// if possivleLength is odd halfLength should be rounded up
			int maxDecreaser = min(possibleLength + 1 - k, k);
			int decreaser = 1;
			int64_t firstPositionForBitboard = binaryPos >> (spaceBottomLeft * (m + 1)+ spaceBottomLeft);
			int firstPositionForHeuristic = normalPos - (spaceBottomLeft * (m + 1) + spaceBottomLeft);
			for (int i = 0; i < possibleLength; i++) // now we want to decrease the values so that only the possible winning sets are represented
			{
				int64_t currentLocationInBitboard = (firstPositionForBitboard << ((i) * (m + 1) +  i));
				int currentLocationForHeuristic = firstPositionForHeuristic + i * (m + 1) + i;

				if (i != spaceBottomLeft && (bitBoardPlayerOne & currentLocationInBitboard) == 0 && (bitBoardPlayerTwo & currentLocationInBitboard) == 0 && (winningSetHeuristic.getPosition(currentLocationForHeuristic) != 0 || isReverse))
				{
					winningSetHeuristic.decreaseWinningSetCount(currentLocationForHeuristic, decreaser * reverseFaktor);
				}
				if (halfLength > i) // first half	
				{
					if (decreaser < maxDecreaser) // raise the decreaser until it reaches kLength or it is over the halfLength
					{
						decreaser++;
					}
				}
				else // second half
				{
					if (possibleLength - i - 1 < maxDecreaser)
					{
						decreaser--;
					}
				}

			}
		}

		///// --------------------------- AntiDiagonal

		int spaceUpLeft = 0;

		for (int i = 1; mPosition - i >= 0 && nPosition + i < n && i < k; i++)
		{
			if ((bitBoardPlayerTwo & (binaryPos << (i * (m + 1) - i))) == 0) // as long as there is no mark of P2, the board does not end and the distance is less then k, a winning set contaning m n could go further to the left
			{
				spaceUpLeft++;
			}
			else
			{
				i = k; // breaks out of the loop
			}
		}
		int spaceDownRight = 0;
		for (int i = 1; nPosition - i >= 0 && mPosition + i < m && i < k; i++)
		{
			if ((bitBoardPlayerTwo & (binaryPos >> (i * (m + 1) - i))) == 0) // as long as there is no mark of P2, the board does not end and the distance is less then k, a winning set contaning m n could go further to the left
			{
				spaceDownRight++;
			}
			else
			{
				i = k; // breaks out of the loop. mLength would have the same effekt here
			}
		}

		if (spaceUpLeft + spaceDownRight + 1 >= k) // if the possible space is uint8_ter then k it is not possible that there was a winning set before
		{
			int possibleLength = spaceUpLeft + spaceDownRight + 1;
			int halfLength = (int)ceil(possibleLength / 2.0f);// if possivleLength is odd halfLength should be rounded up
			int maxDecreaser = min(possibleLength + 1 - k, k);
			int decreaser = 1;
			int64_t firstPositionOnBitboard = binaryPos <<(spaceUpLeft*(m+1)-spaceUpLeft);
			int firstPositionsOnHeuristic = normalPos +(spaceUpLeft*(m+1)-spaceUpLeft);
			for (int i = 0; i < possibleLength; i++) // now we want to decrease the values so that only the possible winning sets are represented
			{
				int64_t currentLocationInBitboard = (firstPositionOnBitboard >> (i * (m + 1)- i));
				int currentLocationOnHeuristic = firstPositionsOnHeuristic - (i * (m + 1) - i);
				if (i != spaceUpLeft && (bitBoardPlayerOne & currentLocationInBitboard) == 0 && (bitBoardPlayerTwo & currentLocationInBitboard) == 0 && (winningSetHeuristic.getPosition(currentLocationOnHeuristic) != 0 || isReverse))
				{
					winningSetHeuristic.decreaseWinningSetCount(currentLocationOnHeuristic, decreaser * reverseFaktor);
				}
				if (halfLength > i) // first half		// Basically cast i to 0 as the length is in dependent of the start position
				{
					if (decreaser < maxDecreaser) // raise the decreaser until it reaches kLength or it is over the halfLength
					{
						decreaser++;
					}
				}
				else // second half
				{
					if (possibleLength - i - 1 < maxDecreaser)
					{
						decreaser--;
					}
				}

			}
		}

	}
}