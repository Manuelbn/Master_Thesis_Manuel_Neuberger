#include <unordered_map>
#include "PNSData.h"
#pragma once

struct PNSTableElement
{
public:
	int64_t playerOneBoard; // The Bitboard for playerOne and two
	int64_t playerTwoBoard;
	int8_t depth;
	bool result; // The outcome of the leaf

	PNSTableElement(unsigned short newDepth, bool newResult, int64_t playOneBitBoard, int64_t playerTwoBitBoard);
};
class PNSTranspositionTable
{
public:

	int tableSize;
	std::unordered_map<int64_t, PNSTableElement> transpositionTable;

	PNSTranspositionTable(int boardSize);

	pair<int64_t, int64_t> getLowestKey(int64_t player1Board, int64_t player2Board, PNSData& data);
	bool find(int64_t player1board, int64_t player2board, PNSData& data);
	
	int64_t makeBoardsToHash(int64_t player1, int64_t player2);

	bool resultOfLastLookUp;
	void insert(bool result, int64_t player1, int64_t player2, int depth, PNSData& data); // should be called only over insertAllMirroedKeyIntoHashTabelle

};

