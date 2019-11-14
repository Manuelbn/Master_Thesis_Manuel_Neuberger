#include "iostream"
#include <unordered_map>
#include "Board.h"
#include <utility> 
#pragma once

struct TableElement
{
	public:
	int64_t playerOneBoard; // The Bitboard for playerOne and two
	int64_t playerTwoBoard;
	int8_t depth;
	bool result;

	TableElement(int8_t newDepth, bool newResult, int64_t playOneBitBoard, int64_t playerTwoBitBoard);
};


class TranspositionTable
{
	public:
		
		
	std::unordered_map<int64_t, TableElement> transpositionTable;

	TranspositionTable(int boardSize);

	pair<int64_t,int64_t> getLowestKey(Board& gameBoard);
	bool find(Board& gameBoard);
	// As many boards can be idential if they are roteted etc. we get the smallest value of this rotations/ mirrorings before we put it into the Transposition Table
	void insertAllMirroredKeysIntoHashTabelle(Board& gameBoard, bool result); // As many boards can be idential if they are roteted etc. we get the smallest value of this rotations/ mirrorings before we put it into the Transposition Table
		


		// Functions to mirror the board to check for symmetrie
		int64_t mirrorKeyHorizontal(int64_t playerKey, Board& gameBoard);
		int64_t mirrorKeyVertical(int64_t playerKey, Board& gameBoard);
		int64_t mirrorKeyDiagonal(int64_t playerKey, Board& gameBoard);
		void insert(bool result, Board& gameBoard); // should be called only over insertAllMirroedKeyIntoHashTabelle
};

