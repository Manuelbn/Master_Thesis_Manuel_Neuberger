#pragma once
#include <cstdint>
#include <vector>
#include "Heuristic.h"
#include <iostream>
#include <algorithm>
#include <cmath> 
using namespace std;

struct MirroringBitMasks
{
	// These Bitmask are used to quickly find out if a board is symmetrik. Every positions in the array is one, row/collom/diagonalrow (depending on the kinde of symmetrie). Many entries may be emtpy but to compute the board on the stack we keep them.
// for a 3*3 board, bitMaskesForHorizontalSymmetry would look like this:
// noraml board					Mask
// 256	512 1024	| 2048		[2] -> 1792
// 16	32	64		| 128		[1] ->  112
// 1	2	4		| 8			[0] ->    7
	int64_t bitMasksForHorizontalMirroring[8];
	int64_t bitMasksForVerticalMirroring[8];
	int64_t bitMasksForDiagonalMirroringOne[8];
	int64_t bitMasksForDiagonalMirroringTwo[8];
	uint8_t sizeOfBitMaskVetors = 8;

};

struct SymmetryBitMasks
{	// These Bitmasks are for the excusion of move when adding new moves
	// They are simple the sum of all rows that are allowed
	// e.g. for the 3*3 board the bitMaskForHorizontalSymmetryOnlyBottomHalf is 7+112 =119. As the board is symmetric in the horizontal the top row can be ignored (the moves do not need to be added).
	int64_t bitMaskForHorizontalSymmetryOnlyBottomHalf;
	int64_t bitMaskForVerticalSymmetryOnlyLeftHalf;
	int64_t bitMaskForDiagonalSymmetryOnlyBottomHalf;
	int64_t bitMaskForAntiDiagonalSymmetryOnlyBottomHalf;
};

class Board
{
public:
	// Generell board variables
	int64_t k;// The number of marks which need to be connected to win the game.
	int64_t m, n; // m, n, side length of the game board
	int64_t boardDepth = 0;
	bool isSquare;
	bool debugMode =0 ; // Is set to true it presents many debug informations. But it make the code a lot slower.

	
	vector<pair<int64_t, uint16_t>> freePositions; // The move that can currently be played
	// Enum for the winDeptharray 
	enum isWon { lost, unclear, won, }; // Value type for the windeptharray

	// Debugging values
	int64_t turnId; // Counts the number of moves
	int transpositionTableHits;

	// The bitboards
	int64_t bitBoardPlayerOne = 0b0; // The board of Player one
	int64_t bitBoardPlayerTwo = 0b0; // The board of Player two

	// TranspositionTables variables
	bool resultOfTableLookUp;

	// These numbers are used to check if a player has won the game. See the wincheck for why they are importend
	int64_t diffHorizontal =1;
	int64_t diffVertical;
	int64_t diffDiagonal;
	int64_t diffAntidiagonal;
	// Konstruktor
	Board(int m, int n, int k);

	// Getter funktions
	int getM();
	int getN();
	int getK();


	// Bitboard funktions
	// INitalises the diffvalues for the wincheck
	void initaliseDiffValues(int64_t m, int64_t n);
	// Adds a move to the board of a player.
	void addMove(int64_t x,bool playerOnesTurn);
	// Remove a move from a board
	void removeMove(int64_t x, bool isPlayerOne);
	// Checks if a player has won
	bool winCheck(bool playerOneCheck); // Checks if the currentBoard is won for playerOne if playerOneCheck is true and for playerTwo if it is false.
	bool winCheck(int64_t board); // Checks if the board "board" is won

	// Funktion for Hashing
	int64_t makeBitBoardsToHash(int64_t player1, int64_t player2);

	// BitMasks
	MirroringBitMasks mirroringBitMasks;
	SymmetryBitMasks symmetryBitMasks;
	


	// Functions to mirror the board to check for symmetrie
	int64_t mirrorKeyHorizontal(int64_t playerKey, Board& gameBoard);
	int64_t mirrorKeyVertical(int64_t playerKey, Board& gameBoard);
	int64_t mirrorKeyDiagonal(int64_t playerKey, Board& gameBoard);

	void fillBitMasks();


	// Heuristics
	// Updates the field after the move m n. The isReverse variabel handles if the values are added or suptracted
	void updateHeuristicField(bool isPlayerOne, bool isReverse, int dataForReverse, int64_t binaryPos, int normalPos);
	
	Heuristic winningSetHeuristic;


	// Variables for ForcedMoves
	int forcedMovePos;
	bool nextMoveIsForced;

	// Debug functions
	void printGameBoard(); // Prints the gameboard. This is a combination of the playerOne and Two bitarray

};

