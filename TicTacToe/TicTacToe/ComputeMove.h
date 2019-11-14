#pragma once
#include "board.h"
#include "ReverseMove.h"
#include "iostream"
#include "TranspositionTable.h"
#include <stack>

// This class is a wrapper for the computeMove method (the other methods are all called from there). It can also add new elements to the moveStack
class ComputeMove
{
public:
	// Constructor/ Destructor
	ComputeMove(int depth, bool currentPlayerIsFirstPlayerNew, int64_t newBinaryPos, int16_t newNormalPos);
	~ComputeMove();
	// Variables
	int64_t computationDepth; // The depth in the search tree of the game
	int64_t binaryPos; // Position which shell be played. The number is in binary so the first numbers are 2^0, 2^1, 2^2 etc. For this number the dead position also counts.
	uint8_t normalPos; // Position which is changed in the Heuristic. This counts normal and ignores the dead area.
	bool currentPlayerisFirstPlayer; // Whos turn is it? True = P1, False = P2.


	// This is the central function. It evalutes the current move adds new moves etc.
	void computeMove(stack<ReverseMove>& reverseStack, stack<ComputeMove>& moveStack, Board& gameBoard, vector<Board::isWon>& winDepthArray, TranspositionTable& transpositiontable); // See class description


	// Checks if "value" is better for the current players than the entry in the winDepthArray and if so updates it. Every entry is better then unclear. If win or lose is better depends on the current player (minimax)
	void updateWinDepthArray(Board::isWon value, vector<Board::isWon>& winDepthArray, int currentComputationDepth);

	// Checks if the game is over for any reason (draw or a player can defenatlly win). If so it sets the values in the winDepthArray
	bool canGameResultBeDetermined(vector<Board::isWon>& winDepthArray, Board& gameBoard, stack<ReverseMove>& reverseStack, TranspositionTable& TranspositionTable);
		// If the currentplayer (definded by the "isPlayerOne" variable) can win by playing one move the function returns true otherwise false.
		bool gameCanBeWonNextMove(Board & gameBoard, bool isPlayerOne);


	// adds the new moves to the stack. The number of moves depends on symmetrys and forced moves
	void addNewMoves(stack<ComputeMove>& moveStack, Board& gameBoard, TranspositionTable& transpositiontable);

	
};

