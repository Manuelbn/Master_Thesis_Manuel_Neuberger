#include "stdafx.h"
#include "PNSNode.h"

// Default constructor
PNSNode::PNSNode()
{
}

// Normal constructors
PNSNode::PNSNode(int64_t playerOneBoardNew, int64_t playerTwoBoardNew, bool isPlayerOneNew, PNSNode* newParent, int8_t oldDepth, PNSData& data)
{
	playerOneBoard = playerOneBoardNew;
	playerTwoBoard = playerTwoBoardNew;
	currentPlayerisOne = isPlayerOneNew;
	parent = newParent;
	depth = oldDepth + 1;

	isExpanded = false;
	isSolved = false;

	initializePNandDN(data);
}

// Root constructor
PNSNode::PNSNode(int64_t playerOneBoardNew, int64_t playerTwoBoardNew, bool isPlayerOneNew, int8_t oldDepth)
{
	playerOneBoard = playerOneBoardNew;
	playerTwoBoard = playerTwoBoardNew;
	currentPlayerisOne = isPlayerOneNew;
	depth = oldDepth + 1;

	isExpanded = false;
	isSolved = false;

	// As the root is the only node at the beginning it will be evaluated independent of PN and DN
	proofNumber = 1;
	disproofNumber = 1;
	parent = NULL;

}
void PNSNode::initializePNandDN(PNSData& data)
{
	// to initalize the proof and disproof number we check how long the longest winning set on the board currently is. The value are set accordingly
	
	int64_t	horizontalWin = playerOneBoard;
	int64_t	verticalWin = playerOneBoard;
	int64_t	diagonalWinOne = playerOneBoard;
	int64_t	diagonalWinTwo = playerOneBoard;
	int64_t longestWinningRow=1;
	for (int i = 0; i < data.k - 1; i++)
	{
		horizontalWin = horizontalWin & (horizontalWin >> data.diffHorizontal);
		verticalWin = verticalWin & (verticalWin >> data.diffVertical);
		diagonalWinOne = diagonalWinOne & (diagonalWinOne >> data.diffDiagonal);
		diagonalWinTwo = diagonalWinTwo & (diagonalWinTwo >> data.diffAntiDiagonal);
		if (horizontalWin != 0 || verticalWin != 0 || diagonalWinOne != 0 || diagonalWinTwo != 0)
		{
			longestWinningRow++;
		}
		else
		{
			break;
		}
	}

	proofNumber = data.k-longestWinningRow;
	disproofNumber = longestWinningRow;
}
void PNSNode::expandNode(PNSData& data, PNSTranspositionTable& transpositionTable)
{
	if (transpositionTable.find(playerOneBoard,playerTwoBoard, data))
	{
		// If this game has allready been solved we can insert the value
		isSolved = true;
		if (transpositionTable.resultOfLastLookUp) // the result of last Look up varialbel saves wether the game was won or lost.
		{
			// Game won for P1
			proofNumber = 0;
			disproofNumber = -1;

		}
		else
		{
			proofNumber = -1;
			disproofNumber = 0;
		}
	}
	else
	{
		if (!gameCanBeEnded(data,transpositionTable ) && !findAndPerformForcedMove(data,transpositionTable ))
		{
			addChildren(data,transpositionTable );
		}
	}
	// We iterate over all ancestors of the node
	PNSNode* nextParent = this;
	while (nextParent->depth > 0) // we iterate over all ancestors until we reach the root
	{
		if (!nextParent->isSolved)  // If the current node has been solved it does not have children and can there for not be updated. This is relevant for the current node
		{
			nextParent->updatePNandDN(data, transpositionTable);
		}
	nextParent = nextParent->parent;
	}
	nextParent->updatePNandDN(data,transpositionTable ); // We also update for the root, but it has no parent
}

void PNSNode::updatePNandDN(PNSData& data, PNSTranspositionTable& transpositionTable)
{
	// This method upadtes the proof and disproofnumber of this node.
	// For the depending on the currentplayer either the minimum of the proofNumber and the sum of
	// the disproof numbers is computed or reverse. 

	// every node has at least on child otherwise it would have been solved before
	int64_t currentPNCandidate = children.at(0).proofNumber;
	int64_t currentDNCandidate = children.at(0).disproofNumber;
	// Which players turn it is basiclly determines whether the current node is a AND/OR node.
	if (currentPlayerisOne)
	{		
		for (int i = 1; i < children.size(); i++)
		{
			if ((children.at(i).proofNumber < currentPNCandidate||currentPNCandidate==-1) && children.at(i).proofNumber!=-1)
			{
				currentPNCandidate = children.at(i).proofNumber;
			}
			if (children.at(i).disproofNumber != -1&& currentDNCandidate!= -1)
			{
				currentDNCandidate += children.at(i).disproofNumber;
			}
			else
			{
				currentDNCandidate = -1;
			}

			if (currentDNCandidate == -1) // -1 represents infinity
			{
				break;
			}
		}
	}
	else
	{
		// every node has at least on child otherwise it would have been solved before

		for (int i = 1; i < children.size(); i++)
		{
			if ((children.at(i).disproofNumber < currentDNCandidate||currentDNCandidate==-1)&& children.at(i).disproofNumber!=-1)
			{
				currentDNCandidate = children.at(i).disproofNumber;
			}
			
			if (children.at(i).proofNumber != -1&& currentPNCandidate != -1)
			{
				currentPNCandidate += children.at(i).proofNumber;
			}
			else
			{
				currentPNCandidate = -1;
			}
			if (currentPNCandidate == -1) // -1 represents infinity
			{
				break;
			}
		}
	}
	
	proofNumber = currentPNCandidate;
	disproofNumber = currentDNCandidate;
	// If the PN or DN is 0 or -1 the node is solved. (-1 = inf)
	// Usually if the proofNumber is 0 the disproof number is -1 and reverse. We do all four checks here
	// to prevent possible inconsistensies  
	if (proofNumber == 0 ||proofNumber ==-1|| disproofNumber == -1||disproofNumber==0)
	{
		if (proofNumber == 0)
		{
			transpositionTable.insert(true, playerOneBoard, playerTwoBoard,depth, data);
		}
		else
		{
			transpositionTable.insert(false, playerOneBoard, playerTwoBoard,depth,data);
		}
		isSolved = true;
		children.clear();
	}
}



bool PNSNode::gameCanBeEnded(PNSData& data, PNSTranspositionTable& transpositionTable)
{
	if (drawCheck(data))
	{
		// add result to transposition table
		transpositionTable.insert(false, playerOneBoard, playerTwoBoard, depth,data);
		// Draw
		proofNumber = -1; // -1 as a Int64_t is the same as the largest possible number
		disproofNumber = 0;
		isSolved = true;
		isExpanded = true;
		return true;
	}
	for (int i = 0; i < data.boardSize; i++)
	{
		int64_t mask = (int64_t)1 << i;
		// Checks if one of the children wins the game for the player in this case they do not need to be expanded
		if (i % (data.m + 1) != data.m && (playerOneBoard & mask) == 0 && (playerTwoBoard & mask) == 0) // No Deadpositions, and the move has not been played sofar
		{
			if (currentPlayerisOne)
			{
				if (winCheck(data, playerOneBoard | mask))
				{
					proofNumber = 0;
					disproofNumber = -1;
					isSolved = true;
					
					transpositionTable.insert(true,playerOneBoard,playerTwoBoard,depth,data);
					return true;
				}
			}
			else
			{
				if (winCheck(data, playerTwoBoard | mask))
				{
					proofNumber =-1;
					disproofNumber = 0;
					isSolved = true;

					transpositionTable.insert(false, playerOneBoard, playerTwoBoard, depth,data);
					return true;
				}
			}
		}
	}
	return false;
}

// Checks if the other player can win the game next turn. Note that this methode only makes sence if the current player can not win.
// It returns true if a forced move was found and performed and false if not so
bool PNSNode::findAndPerformForcedMove(PNSData& data, PNSTranspositionTable& transpositionTable)
{
	for (int i = 0; i < data.boardSize; i++)
	{
		int64_t mask = (int64_t)1 << i;
		// Checks if one of the children wins the game for the player in this case they do not need to be expanded
		if (i % (data.m + 1) != data.m && (playerOneBoard & mask) == 0 && (playerTwoBoard & mask) == 0) // No Deadpositions, and the move has not been played sofar
		{
			if (currentPlayerisOne)
			{
				if (winCheck(data, playerTwoBoard | mask))
				{
					children.push_back(PNSNode(playerOneBoard | mask, playerTwoBoard, false, this, depth, data));
					isExpanded = true;
					return true;
				}
			}
			else
			{
				if (winCheck(data, playerOneBoard | mask))
				{
					children.push_back(PNSNode(playerOneBoard, playerTwoBoard | mask, true, this, depth,data));
					isExpanded = true;
					return true;
				}
			}
		}
	}
	return false;
}

void PNSNode::addChildren(PNSData& data, PNSTranspositionTable& transpositionTable)
{
	// Check for Symmetries.
		// Not all symmetry combinations can exists together, so we do not need to allways check all cases. The ones that can exist (on a square board) are:
		// (0. no symmetry)
		// 1-4. One of the four diagonals by it self in this order(horizontal, vertical, diagonal, antidigonal)
		// 5. vertical and horizontal
		// 6. diagonal and antidiagonal
		// 7. All at once
	// In a not square game only the cases 1,2,5 exist
	int symmetryCase = 0;
	
	int64_t basicKeyP1 = playerOneBoard;
	int64_t basicKeyP2 = playerTwoBoard;

	int64_t horizontalMirroredKeyP1 = data.mirrorKeyHorizontal(basicKeyP1);
	int64_t verticalMirroredKeyP1 = data.mirrorKeyVertical(basicKeyP1);
	int64_t diagonalMirroredKeyP1;
	int64_t antiDiagonalMirroedKeyP1;

	// We need to check if both the boards of both P1 and P2 are the same to find symmetrie
	int64_t horizontalMirroredKeyP2 = data.mirrorKeyHorizontal(basicKeyP2);
	int64_t verticalMirroredKeyP2 = data.mirrorKeyVertical(basicKeyP2);
	int64_t diagonalMirroredKeyP2;
	int64_t antiDiagonalMirroedKeyP2;

	if (horizontalMirroredKeyP1 == basicKeyP1 && horizontalMirroredKeyP2 == basicKeyP2) // If this two keys are the same the game is horizontalSymmetrik. Now it needs to be checks which other symmetries exist aswell.
	{
		if (horizontalMirroredKeyP1 != verticalMirroredKeyP1 || horizontalMirroredKeyP2 != verticalMirroredKeyP2)
		{
			symmetryCase = 1; // only horizontalSymmetrik
		}
		else {
			if (data.boardIsSquare)
			{
				diagonalMirroredKeyP1 = data.mirrorKeyDiagonal(basicKeyP1);
				diagonalMirroredKeyP2 = data.mirrorKeyDiagonal(basicKeyP2);
				if (horizontalMirroredKeyP1 != diagonalMirroredKeyP1 || horizontalMirroredKeyP2 != diagonalMirroredKeyP2)
				{
					symmetryCase = 5; // Vertical and Horizontal but not diagonal
				}
				else
				{
					symmetryCase = 7;  // All symmetries
				}
			}
			else
			{
				symmetryCase = 5; // Vertical and Horizontal but not diagonal
			}
		}

	}
	else
		if (basicKeyP1 == verticalMirroredKeyP1 && basicKeyP2 == verticalMirroredKeyP2) // for the non square case this is the last check
		{
			//As the game can not be vertical and horizontal (otherwise the a former case would have triggered) it is only vertical
			symmetryCase = 2;
		}
		else if (data.boardIsSquare)
		{
			diagonalMirroredKeyP1 = data.mirrorKeyDiagonal(basicKeyP1);
			antiDiagonalMirroedKeyP1 = data.mirrorKeyHorizontal(data.mirrorKeyVertical(diagonalMirroredKeyP1)); // To create the anitdiagonal, we take the diagonal, and mirror it both horrizontal and vertical. This could be done quicker by adding a seperate funktion for the antidiagonal.
			diagonalMirroredKeyP2 = data.mirrorKeyDiagonal(basicKeyP2);
			antiDiagonalMirroedKeyP2 = data.mirrorKeyHorizontal(data.mirrorKeyVertical(diagonalMirroredKeyP2)); // To create the anitdiagonal, we take the diagonal, and mirror it both horrizontal and vertical. This could be done quicker by adding a seperate funktion for the antidiagonal.


			if (basicKeyP1 == diagonalMirroredKeyP1 && basicKeyP2 == diagonalMirroredKeyP2)
			{

				if (basicKeyP1 != antiDiagonalMirroedKeyP1 || basicKeyP2 != antiDiagonalMirroedKeyP2)
				{
					symmetryCase = 3;// Only the diagonal
				}
				else
				{
					symmetryCase = 6; // Diagonal and Antidiagonal
				}
			}
			else if (basicKeyP1 == antiDiagonalMirroedKeyP1 && basicKeyP2 == antiDiagonalMirroedKeyP2)
			{
				symmetryCase = 4; //Only Antidiagonal
			}
		}
		
	// Not all symmetry combinations can exists together, so we do not need to allways check all cases. The ones that can exist (on a square board) are:
			// (0. no symmetry)
			// 1-4. One of the four diagonals by it self in this order(horizontal, vertical, diagonal, antidigonal)
			// 5. vertical and horizontal
			// 6. diagonal and antidiagonal
			// 7. All at once
				// Depending on the case only some moves are added
	int64_t legalSymmetryAreaMask = 0;
	switch (symmetryCase)
	{
	case 0:
		// If ther are no symmetries we do nothing
		legalSymmetryAreaMask = -1; // -1 is represented as the integer that has only 1 set
		break;
	case 1: // Horizontal
		legalSymmetryAreaMask = data.bitMaskForHorizontalSymmetryOnlyBottomHalf;
		break;
	case 2:// vertical
		legalSymmetryAreaMask = data.bitMaskForVerticalSymmetryOnlyLeftHalf;
		break;
	case 3: // diagonal
		legalSymmetryAreaMask = data.bitMaskForDiagonalSymmetryOnlyBottomHalf;
		break;
	case 4: // antidiagonal
		legalSymmetryAreaMask = data.bitMaskForAntiDiagonalSymmetryOnlyBottomHalf;
		break;
	case 5: // vertical and horizontal
		legalSymmetryAreaMask = data.bitMaskForHorizontalSymmetryOnlyBottomHalf & data.bitMaskForVerticalSymmetryOnlyLeftHalf;
		break;
	case 6: // Diagonal and antidiagonal
		legalSymmetryAreaMask = data.bitMaskForDiagonalSymmetryOnlyBottomHalf & data.bitMaskForAntiDiagonalSymmetryOnlyBottomHalf;
		break;
	case 7: // All. But we do not add the antidiagonal here as it does not change anything
		legalSymmetryAreaMask = data.bitMaskForHorizontalSymmetryOnlyBottomHalf & data.bitMaskForVerticalSymmetryOnlyLeftHalf & data.bitMaskForDiagonalSymmetryOnlyBottomHalf;
		break;
	}
	// Adds the moves
		for (int i = 0; i < data.boardSize; i++)
		{
			// If one of the children allready solves the parent we do not need to expaned the other children
			if (children.size() != 0 && ((!currentPlayerisOne && children.at(children.size() - 1).proofNumber == 0) || (currentPlayerisOne && children.at(children.size() - 1).proofNumber == 0)))
			{
				break;
			}
			int64_t move = (int64_t)1 << i;
			if ((legalSymmetryAreaMask & move) != 0) // Move is in the symmetrie relevant area
			{
			
				if (i % (data.m + 1) != data.m && (playerOneBoard & move) == 0 && (playerTwoBoard & move) == 0) // No Deadpositions, and the move has not been played sofar
				{
				
					if (currentPlayerisOne)
					{
						children.push_back(PNSNode(playerOneBoard | move, playerTwoBoard, false, this, depth, data));
					}
					else
					{
						children.push_back(PNSNode(playerOneBoard, playerTwoBoard | move, true, this, depth, data));
					}
				}
			}
		}
		isExpanded = true;
}


bool PNSNode::winCheck(PNSData& data)
{// Like in minimax
	int64_t horizontalWin;
	int64_t verticalWin;
	int64_t diagonalWinOne;
	int64_t diagonalWinTwo;
	if (!currentPlayerisOne) // as is is allready the other players move this need to be reverted
	{
		horizontalWin = playerOneBoard;
		verticalWin = playerOneBoard;
		diagonalWinOne = playerOneBoard;
		diagonalWinTwo = playerOneBoard;
	}
	else
	{
		horizontalWin = playerTwoBoard;
		verticalWin = playerTwoBoard;
		diagonalWinOne = playerTwoBoard;
		diagonalWinTwo = playerTwoBoard;
	}
	for (int i = 0; i < data.k - 1; i++)
	{
		horizontalWin = horizontalWin & (horizontalWin >> data.diffHorizontal);
		verticalWin = verticalWin & (verticalWin >> data.diffVertical);
		diagonalWinOne = diagonalWinOne & (diagonalWinOne >> data.diffDiagonal);
		diagonalWinTwo = diagonalWinTwo & (diagonalWinTwo >> data.diffAntiDiagonal);
	}
	return horizontalWin != 0 || verticalWin != 0 || diagonalWinOne != 0 || diagonalWinTwo != 0;
}
bool PNSNode::winCheck(PNSData& data, int64_t bitboard)
{ // like in minimax
	int64_t	horizontalWin = bitboard;
	int64_t	verticalWin = bitboard;
	int64_t	diagonalWinOne = bitboard;
	int64_t	diagonalWinTwo = bitboard;
	for (int i = 0; i < data.k - 1; i++)
	{
		horizontalWin = horizontalWin & (horizontalWin >> data.diffHorizontal);
		verticalWin = verticalWin & (verticalWin >> data.diffVertical);
		diagonalWinOne = diagonalWinOne & (diagonalWinOne >> data.diffDiagonal);
		diagonalWinTwo = diagonalWinTwo & (diagonalWinTwo >> data.diffAntiDiagonal);
	}
	return horizontalWin != 0 || verticalWin != 0 || diagonalWinOne != 0 || diagonalWinTwo != 0;
}

bool PNSNode::drawCheck(PNSData& data)
{
	// By appling the Xor operator to the board of P2 and the mask for the whole board, we get all positions which are not owned by P2. 
	// If playerOne would own all these positions and still can not win the game is definatly a draw.
	return  !winCheck(data,playerTwoBoard^ data.maskOfWholeBoard);
}
