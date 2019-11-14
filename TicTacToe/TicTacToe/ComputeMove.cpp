#include "stdafx.h"
#include "ComputeMove.h"




ComputeMove::~ComputeMove()
{
}
ComputeMove::ComputeMove(int depth, bool currentPlayerIsFirstPlayerNew, int64_t newBinaryPos, int16_t newNormalPos)
{
	currentPlayerisFirstPlayer = currentPlayerIsFirstPlayerNew;
	computationDepth = depth;
	binaryPos = newBinaryPos;
	normalPos = newNormalPos;
}

// Check if in the winFoundForPlayerAtDepth array for the current depth an optimal value was allready found


void ComputeMove::computeMove(stack<ReverseMove>& reverseStack, stack<ComputeMove>& moveStack, Board& gameBoard, vector<Board::isWon>& winDepthArray, TranspositionTable& transpositiontable)
{
	// If an optimalValue was already found we can skip most computations.  This one line is basically alpha and beta pruning
	if (!(currentPlayerisFirstPlayer && winDepthArray[computationDepth] == Board::isWon::won) && !(!currentPlayerisFirstPlayer && winDepthArray[computationDepth] == Board::isWon::lost))
	{
		// Reverts the board into the correct state
		while (!(reverseStack.empty()) && computationDepth < (gameBoard.boardDepth+1)) // As the current move has not jet been added we need to take boardDepth +1 to reverse the board to one deapth further
		{
			ReverseMove rev = reverseStack.top();
			rev.reverse(gameBoard);
			reverseStack.pop();

			// Debugging
			if (gameBoard.debugMode)
			{
				cout << "reversed board "<<gameBoard.turnId << endl;
				gameBoard.winningSetHeuristic.printHeuristicField();
				gameBoard.printGameBoard();
			}
			// Adding reverted moves to the transposition table
			if (computationDepth < gameBoard.boardDepth+1) // while reversing the last move must not be added, as this is the board the current move we are trying to prove.
			{
				if (winDepthArray[computationDepth] == Board::isWon::won)
				{
					transpositiontable.insert(true,gameBoard);
				}
				if (winDepthArray[computationDepth] == Board::isWon::lost)
				{
					transpositiontable.insert(false,gameBoard);
				}
			}
		}
		// Checks if the programm is still running
		if (gameBoard.turnId % 10000000 == 0&& gameBoard.turnId!=0) 
		{
			cout << "I am still working at move "<<gameBoard.turnId << endl;
		}

		// sets the mark
		gameBoard.addMove(binaryPos, currentPlayerisFirstPlayer);
		// Remove played move from the freePositions vector
		gameBoard.freePositions.erase(std::find(gameBoard.freePositions.begin(), gameBoard.freePositions.end(), pair<int64_t,uint16_t> (binaryPos,normalPos)));
		
		// Heuristic
		uint8_t winningSetAmountForReverse = gameBoard.winningSetHeuristic.getPosition(normalPos); // As the updateheuristics will remove the value it has to be saved here
		gameBoard.updateHeuristicField(currentPlayerisFirstPlayer,false,0,binaryPos,normalPos); // updates the Heuristic field.  the reverse data variiable is not used
		
		
		if (gameBoard.debugMode)
		{	
			cout << "normal board " << gameBoard.turnId << endl;
			gameBoard.printGameBoard();
			gameBoard.winningSetHeuristic.printHeuristicField();
		}
		// The turnId just counts how many move have been performed yet
		gameBoard.turnId++;

		// adds the action to the reverseStack so it can be undone later
		reverseStack.push(ReverseMove(binaryPos, computationDepth, currentPlayerisFirstPlayer, winningSetAmountForReverse, normalPos));

	
		// If for the current gameboard a hash exists in the table this means that the gameboard has allready been solved. The winDepthArry is updated and no new moves are added
		//If no transpositionsTabelles are used, proceed with the else part.
		if (transpositiontable.find(gameBoard))
		{
			Board::isWon w;
			if (gameBoard.resultOfTableLookUp == true)
			{
				if (gameBoard.debugMode)
				{
					cout << "win" << endl;
				}
				w = Board::isWon::won;
			}
			else
			{
				if (gameBoard.debugMode)
				{
					cout << "lose or draw" << endl;
				}
				w = Board::isWon::lost;
			}
			updateWinDepthArray(w, winDepthArray, computationDepth);
		}
		else
		{
			// Checks if the game can be ended by the other player.
			if(!canGameResultBeDetermined(winDepthArray, gameBoard, reverseStack, transpositiontable)) 
			{
				addNewMoves(moveStack, gameBoard, transpositiontable);
			}
		}
	}
	//else
	{
		if (gameBoard.debugMode)
		{
			cout << "Pruned move " << binaryPos<<" at depth "<<computationDepth << endl;
		}
	}

	// Keeps winDeptharray consistent
	// If the element is the last of the current depth we move the value from winFoundForPlayerAtDepth at the current computationDepth one layer up.
	// This is an essantial part for the minimax algrithm
	int64_t checkDepth = computationDepth;
	while ((moveStack.empty() || moveStack.top().computationDepth < checkDepth) && checkDepth>0)
	{
		updateWinDepthArray(winDepthArray[checkDepth],winDepthArray, checkDepth - 1);
		winDepthArray[checkDepth] = Board::isWon::unclear;
		checkDepth--;
	}
}



void ComputeMove::addNewMoves(stack<ComputeMove>& moveStack, Board& gameBoard,TranspositionTable& transpositiontable)
{
	// Relevant for forced moves
	// Check if the current player could win in the next move (if so the other player would prevent it and does not have to search for different moves)
	if (gameBoard.nextMoveIsForced)
	{
		moveStack.push(ComputeMove(gameBoard.boardDepth + 1, !currentPlayerisFirstPlayer, gameBoard.freePositions.at(gameBoard.forcedMovePos).first,gameBoard.freePositions.at(gameBoard.forcedMovePos).second));
		return;
	}
		// Check for Symmetries.
		// Not all symmetry combinations can exists together, so we do not need to allways check all cases. The ones that can exist (on a square board) are:
		// (0. no symmetry)
		// 1-4. One of the four diagonals by it self in this order(horizontal, vertical, diagonal, antidigonal)
		// 5. vertical and horizontal
		// 6. diagonal and antidiagonal
		// 7. All at once
	// In a not square game only the cases 1,2,5 exist
	
	int symmetryCase = 0;
	
	int64_t basicKeyP1 =gameBoard.bitBoardPlayerOne;
	int64_t basicKeyP2 = gameBoard.bitBoardPlayerTwo;

		int64_t horizontalMirroredKeyP1 = transpositiontable.mirrorKeyHorizontal(basicKeyP1,gameBoard);
		int64_t verticalMirroredKeyP1 = transpositiontable.mirrorKeyVertical(basicKeyP1, gameBoard);
		int64_t diagonalMirroredKeyP1;
		int64_t antiDiagonalMirroedKeyP1;

		// We need to check if both the boards of both P1 and P2 are the same to find symmetrie
		int64_t horizontalMirroredKeyP2 = transpositiontable.mirrorKeyHorizontal(basicKeyP2, gameBoard);
		int64_t verticalMirroredKeyP2 = transpositiontable.mirrorKeyVertical(basicKeyP2, gameBoard);
		int64_t diagonalMirroredKeyP2;
		int64_t antiDiagonalMirroedKeyP2;

		if(horizontalMirroredKeyP1==basicKeyP1&& horizontalMirroredKeyP2 == basicKeyP2) // If this two keys are the same the game is horizontal symmetrik. Now it needs to be checks which other symmetries exist aswell.
		{
			if (horizontalMirroredKeyP1 != verticalMirroredKeyP1|| horizontalMirroredKeyP2 != verticalMirroredKeyP2)
			{
				symmetryCase = 1; // only horizontalSymmetrik
			}
			else {
				if (gameBoard.isSquare)
				{
					diagonalMirroredKeyP1 = transpositiontable.mirrorKeyDiagonal(basicKeyP1, gameBoard);
					diagonalMirroredKeyP2 = transpositiontable.mirrorKeyDiagonal(basicKeyP2, gameBoard);
					if (horizontalMirroredKeyP1 != diagonalMirroredKeyP1 ||horizontalMirroredKeyP2 != diagonalMirroredKeyP2)
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
			if (basicKeyP1 == verticalMirroredKeyP1&&basicKeyP2 == verticalMirroredKeyP2) // for the non square case this is the last check
			{
				//As the game can not be vertical and horizontal (otherwise the a former case would have triggered) it is only vertical
				symmetryCase = 2;
			}
		else if(gameBoard.isSquare)
		{
				diagonalMirroredKeyP1 = transpositiontable.mirrorKeyDiagonal(basicKeyP1, gameBoard);
				antiDiagonalMirroedKeyP1 = transpositiontable.mirrorKeyHorizontal(transpositiontable.mirrorKeyVertical(diagonalMirroredKeyP1, gameBoard),gameBoard); // To create the anitdiagonal, we take the diagonal, and mirror it both horrizontal and vertical. This could be done quicker by adding a seperate funktion for the antidiagonal.
				diagonalMirroredKeyP2 = transpositiontable.mirrorKeyDiagonal(basicKeyP2, gameBoard);
				antiDiagonalMirroedKeyP2 = transpositiontable.mirrorKeyHorizontal(transpositiontable.mirrorKeyVertical(diagonalMirroredKeyP2, gameBoard), gameBoard); // To create the anitdiagonal, we take the diagonal, and mirror it both horrizontal and vertical. This could be done quicker by adding a seperate funktion for the antidiagonal.


				if (basicKeyP1 == diagonalMirroredKeyP1&& basicKeyP2 == diagonalMirroredKeyP2)
				{

					if (basicKeyP1 != antiDiagonalMirroedKeyP1|| basicKeyP2 != antiDiagonalMirroedKeyP2)
					{
						symmetryCase = 3;// Only the diagonal
					}
					else
					{
						symmetryCase = 6; // Diagonal and Antidiagonal
					}
				}
				else if (basicKeyP1 == antiDiagonalMirroedKeyP1&& basicKeyP2 == antiDiagonalMirroedKeyP2)
				{
					symmetryCase = 4; //Only Antidiagonal
				}
		}
		
		// Now we sort using the heuristic
		// using lambda to compare elements.
		sort(gameBoard.freePositions.begin(),gameBoard.freePositions.end(), [&gameBoard](pair<int64_t,uint8_t> a, pair<int64_t, uint8_t>  b)
			{
				if (gameBoard.winningSetHeuristic.getPosition(a.second) != gameBoard.winningSetHeuristic.getPosition(b.second))
				{
					return gameBoard.winningSetHeuristic.getPosition(a.second) < gameBoard.winningSetHeuristic.getPosition(b.second);
				}
				return a.second > b.second;
			});


// Not all symmetry combinations can exists together, so we do not need to allways check all cases. The ones that can exist (on a square board) are:
		// (0. no symmetry)
		// 1-4. One of the four diagonals by it self in this order(horizontal, vertical, diagonal, antidigonal)
		// 5. vertical and horizontal
		// 6. diagonal and antidiagonal
		// 7. All at once
			// Depending on the case only some moves are added

int64_t mask = 0;
switch (symmetryCase)
{
case 0:
	// If ther are no symmetries we do nothing
	mask = -1; // -1 is represented as the integer that has only 1 set
	break;
case 1: // Horizontal
	mask = gameBoard.symmetryBitMasks.bitMaskForHorizontalSymmetryOnlyBottomHalf;
	break;
case 2:// vertical
	mask = gameBoard.symmetryBitMasks.bitMaskForVerticalSymmetryOnlyLeftHalf;
	break;
case 3: // diagonal
	mask =  gameBoard.symmetryBitMasks.bitMaskForDiagonalSymmetryOnlyBottomHalf;
	break;
case 4: // antidiagonal
	mask = gameBoard.symmetryBitMasks.bitMaskForAntiDiagonalSymmetryOnlyBottomHalf;
	break;
case 5: // vertical and horizontal
	mask =  gameBoard.symmetryBitMasks.bitMaskForHorizontalSymmetryOnlyBottomHalf & gameBoard.symmetryBitMasks.bitMaskForVerticalSymmetryOnlyLeftHalf;
	break;
case 6: // Diagonal and antidiagonal
	mask = gameBoard.symmetryBitMasks.bitMaskForDiagonalSymmetryOnlyBottomHalf & gameBoard.symmetryBitMasks.bitMaskForAntiDiagonalSymmetryOnlyBottomHalf;
	break;
case 7: // All. But we do not add the antidiagonal here as it does not change anything
	mask =  gameBoard.symmetryBitMasks.bitMaskForHorizontalSymmetryOnlyBottomHalf & gameBoard.symmetryBitMasks.bitMaskForVerticalSymmetryOnlyLeftHalf & gameBoard.symmetryBitMasks.bitMaskForDiagonalSymmetryOnlyBottomHalf;
	break;

default:
	cout << "Error in add Move" << endl;
	break;
}

		// If symmetry is ignored this is the only importent part. Für the mask variable just assume the default case 0.
		for (int i = 0; i < gameBoard.freePositions.size(); i++)
		{	
			if ((gameBoard.freePositions.at(i).first & mask) != 0)//&& gameBoard.winningSetHeuristik.getPosition(gameBoard.freePositions.at(i).second)!=0)
			{
				moveStack.push(ComputeMove(gameBoard.boardDepth + 1, !currentPlayerisFirstPlayer, gameBoard.freePositions.at(i).first, gameBoard.freePositions.at(i).second));
			}
		}
}



// Checks if the game is over => won or a draw. If so it sets the values of the winDepthArray array
bool ComputeMove::canGameResultBeDetermined(vector<Board::isWon>& winDepthArray, Board& gameBoard, stack<ReverseMove>& reverseStack, TranspositionTable& transpositiontable)
{	
	// If a player can win by a move this is his optimal move. As the current Player has allready played we are looking at the other player
	if (gameCanBeWonNextMove(gameBoard, !currentPlayerisFirstPlayer))
	{
		if (!currentPlayerisFirstPlayer) // As we look at the next move we invert the bool
		{
			updateWinDepthArray(Board::isWon::won, winDepthArray,computationDepth);
			if (gameBoard.debugMode)
			{
				cout << "win" << endl;
			}
			transpositiontable.insert(true,gameBoard);
		}
		else
		{
			updateWinDepthArray(Board::isWon::lost, winDepthArray, computationDepth);
			if(gameBoard.debugMode)
			{
				cout << "lose" << endl;
			}
			transpositiontable.insert(false,gameBoard );
		}
		return true;
	}

	// If the game board is full it is a draw. A draw is viewed like a player 2 victory
	if (gameBoard.winningSetHeuristic.noMoreMoves())
	{
		updateWinDepthArray(Board::isWon::lost, winDepthArray, computationDepth);
		transpositiontable.insert(false,gameBoard);
		if (gameBoard.debugMode)
		{
			cout << "draw" << endl;
		}
		return true;
	}

	// Needed for forcedmoves.

	gameBoard.nextMoveIsForced = false;
	// Does the otherplayer need to prevent a win by the current player?
	if (gameCanBeWonNextMove(gameBoard, currentPlayerisFirstPlayer))
	{
	gameBoard.nextMoveIsForced = true;
	return false;
	}
	return false;
}

// UpdatesWinDepthArray according to Minimax
void ComputeMove::updateWinDepthArray(Board::isWon value, vector<Board::isWon>& winDepthArray, int currentComputationDepth)
{
	if (currentComputationDepth%2==1) // This means that its player X turn
	{
		// If no result was given yet every result is excepted. If the current result is lose it can not get worse, so also every result is excepted.
		if (winDepthArray[currentComputationDepth] == Board::isWon::unclear || winDepthArray[currentComputationDepth] == Board::isWon::lost)
		{
			winDepthArray[currentComputationDepth] = value;
		}
	}
	else // if the Mark is O
	{
		// Same as above but reverted
		if (winDepthArray[currentComputationDepth] == Board::isWon::unclear || winDepthArray[currentComputationDepth] == Board::isWon::won)
		{
			winDepthArray[currentComputationDepth] = value;
		}
	}
}

// Performs a wincheck for all possible moves, to find out if a player can/could win
bool ComputeMove::gameCanBeWonNextMove(Board& gameBoard, bool isPlayerOne)
{
	int64_t board;
	if (isPlayerOne)
	{
		board = gameBoard.bitBoardPlayerOne;
	}
	else
	{
		board = gameBoard.bitBoardPlayerTwo;
	}
	for (int i = 0; i < gameBoard.freePositions.size(); i++)
	{
		if (gameBoard.winCheck(board | gameBoard.freePositions.at(i).first))
		{
			gameBoard.forcedMovePos = i; 
			return true;
		}
	}
	return false;
}

