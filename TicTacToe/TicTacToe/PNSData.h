#pragma once
#include <vector>
using namespace std;
class PNSData
{
	public:
	int8_t m;
	int8_t n;
	int8_t k;
	int8_t boardSize;
	PNSData(int8_t m,int8_t n,int8_t k);
	
	bool boardIsSquare;// the board is square
	
	// Diff values for wincheck. 
	int8_t diffHorizontal;
	int8_t diffVertical;
	int8_t diffDiagonal;
	int8_t diffAntiDiagonal;

	// Functions to mirror the board to check for symmetrie
	void fillBitMasks();
	int64_t mirrorKeyHorizontal(int64_t playerKey);
	int64_t mirrorKeyVertical(int64_t playerKey);
	int64_t mirrorKeyDiagonal(int64_t playerKey);
	// Symmetrie bitmasks
	int64_t bitMaskesForHorizontalSymmetry[8];
	int64_t bitMaskesForVerticalSymmetry[8];
	int64_t bitMaskesForDiagonalSymmetryLeftToRight[8];
	int64_t bitMaskesForDiagonalSymmetryRightToLeft[8];
	
	// part field bitmasks
	// These Bitmasks are for the excusion of move when adding new moves
	// They are simple the sum of all rows that are allowed
	// e.g. for the 3*3 board the bitMaskForHorizontalSymmetryOnlyBottomHalf is 7+112 =119. As the board is symmetric in the horizontal the top row can be ignored (the moves do not need to be added).
	int64_t bitMaskForHorizontalSymmetryOnlyBottomHalf;
	int64_t bitMaskForVerticalSymmetryOnlyLeftHalf;
	int64_t bitMaskForDiagonalSymmetryOnlyBottomHalf;
	int64_t bitMaskForAntiDiagonalSymmetryOnlyBottomHalf;

	int64_t maskOfWholeBoard; // Whole board with out dead positions
	
private:
	void initaliseDiffValues();

	
};

