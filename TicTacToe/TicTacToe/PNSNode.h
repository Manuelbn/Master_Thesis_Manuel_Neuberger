#pragma once
#include <vector>
#include <memory>
#include "PNSData.h"
#include "PNSTranspositionTable.h"
using namespace std;
class PNSNode
{
	public:
	PNSNode();
	PNSNode(int64_t playerOneBoardNew, int64_t playerTwoBoardNew, bool isPlayerOneNew, PNSNode* parent, int8_t oldDepth, PNSData& data);
	// Root constructor
	PNSNode(int64_t playerOneBoardNew, int64_t playerTwoBoardNew, bool isPlayerOneNew, int8_t oldDepth);
	//// Basic variables
	int64_t playerOneBoard;
	int64_t playerTwoBoard;

	bool currentPlayerisOne;// If it is not an ORNode it is an ANDnode. Player X is OR Player O is AND
	int8_t depth;

	// PNS variables
	int64_t proofNumber;
	int64_t disproofNumber;

	bool isSolved; // is the node saved yet
	bool isExpanded; // Has this node been expended yet

	vector<PNSNode> children; // Is emtpy until the node is expanded
	
	PNSNode* parent;


	// Basic Methods
	void expandNode(PNSData& data, PNSTranspositionTable& transpositionTable); // creates the children
	void updatePNandDN(PNSData& data, PNSTranspositionTable& transpositionTable);
	
	void initializePNandDN(PNSData& data);
private:

	bool gameCanBeEnded(PNSData& data, PNSTranspositionTable& transpositiontable);
	bool findAndPerformForcedMove(PNSData& data, PNSTranspositionTable& transpositionTable);
	void addChildren(PNSData& data, PNSTranspositionTable& transpositionTable);
	bool winCheck(PNSData& data);
	bool winCheck(PNSData& data,int64_t bitboard);
	bool drawCheck(PNSData& data);
};

