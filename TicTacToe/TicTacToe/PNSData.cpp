#include "stdafx.h"
#include "PNSData.h"

PNSData::PNSData(int8_t mNew, int8_t nNew, int8_t kNew)
{
	m = mNew;
	n = nNew;
	k = kNew;
	boardSize = (m + 1) * n;
	initaliseDiffValues();
	fillBitMasks();
	if (m == n)
	{
		boardIsSquare =true;
	}
	else 
	{
		boardIsSquare = false;
	}

}

void PNSData::initaliseDiffValues()
{
	diffHorizontal = 1;
	diffVertical = m + 1;
	diffDiagonal = diffVertical + 1;
	diffAntiDiagonal = m;
}



void PNSData::fillBitMasks()
{
	// HorrizontalMask
	int64_t initialHorrizontalMask = (int64_t)pow(2, m) - 1;
	for (int i = 0; i < n; i++)
	{
		bitMaskesForHorizontalSymmetry[i] = initialHorrizontalMask << i * (m + 1);
	}
	// verticalMask
	int64_t initialVerticalMask = 0;
	for (int i = 0; i < n; i++)
	{
		initialVerticalMask += (int64_t)pow(2, i * (m + 1));
	}

	for (int i = 0; i < m; i++)
	{
		bitMaskesForVerticalSymmetry[i] = initialVerticalMask << i;
	}
	// diaginalMask - The diagonal starts in the left bottom corner and then goes up to the right upper corner.
	// For the diagonal mirroring two arraies are used for mirroring form the left to the right and reverse (for one mirroring action we need to applie both once)
	int64_t nextMask;
	// LeftToRight
	for (int i = 0; i < n; i++)
	{
		nextMask = 0;
		for (int j = 0; j < n - i; j++)
		{
			nextMask += (int64_t)pow(2, (i + j) * (m + 1) + j);
		}
		bitMaskesForDiagonalSymmetryLeftToRight[i] = nextMask;
	}
	// RightToLeft
	for (int i = 0; i < n; i++)
	{
		nextMask = 0;
		for (int j = 0; j < n - i; j++)
		{
			nextMask += (int64_t)pow(2, (j) * (m + 1) + i + j);
		}
		bitMaskesForDiagonalSymmetryRightToLeft[i] = nextMask;
	}


	// Fill part field bitmasks 
// horizontal
	bitMaskForHorizontalSymmetryOnlyBottomHalf = 0;
	for (int i = 0; i < (n / 2 + n % 2); i++)
	{
		bitMaskForHorizontalSymmetryOnlyBottomHalf += bitMaskesForHorizontalSymmetry[i];
	}
	// vertical
	bitMaskForVerticalSymmetryOnlyLeftHalf = 0;
	for (int i = 0; i < m/2+m%2; i++)
	{
		bitMaskForVerticalSymmetryOnlyLeftHalf += bitMaskesForVerticalSymmetry[i];
	}
	// diagonal
	bitMaskForDiagonalSymmetryOnlyBottomHalf = 0;
	for (int i = 0; i < n; i++)
	{
		bitMaskForDiagonalSymmetryOnlyBottomHalf += bitMaskesForDiagonalSymmetryRightToLeft[i];
	}
	// antidiagonal
	bitMaskForAntiDiagonalSymmetryOnlyBottomHalf = mirrorKeyVertical(bitMaskForDiagonalSymmetryOnlyBottomHalf); // Instead of mirroring antidiagonal we can also apply the other three mirrorings


	// Mask of the whole board
	maskOfWholeBoard = 0;
	for (int i = 0; i < (m + 1) * n; i++)
	{
		if (i % (m + 1) != m) // Elimanates the dead row
			maskOfWholeBoard += (int64_t)1 << i;
	}

}

int64_t PNSData::mirrorKeyHorizontal(int64_t playerKey)
{
	// To mirror the board in the horizontal we basically isolate each row and shift it to the new position.
	int nhalf = n / 2;
	int64_t horizonalMirroringKey = 0;
	for (int i = 0; i < n; i++) // N is also the size of the relevant bitmasks
	{
		if (i < nhalf)
		{
			horizonalMirroringKey |= (playerKey & bitMaskesForHorizontalSymmetry[i]) << (n - 2 * i - 1) * (m + 1);
		}
		else if (i > nhalf || (i >= nhalf && n % 2 == 0)) // In the case of N is odd the middle row is not changed
		{
			horizonalMirroringKey |= (playerKey & bitMaskesForHorizontalSymmetry[i]) >> (2 * i - n + 1)* (m + 1);
		}
		else
			if (i == nhalf && n % 2 == 1)
			{
				//if the gameboard s is odd, the middle row has to be added as well. The row is unchanged
				horizonalMirroringKey |= playerKey & bitMaskesForHorizontalSymmetry[i];
			}
	}
	return horizonalMirroringKey;
}

int64_t PNSData::mirrorKeyVertical(int64_t playerKey)
{
	// To mirror the board in the verical we basically isolate each column and shift it to the new position.
	int64_t verticalMirroringKey = 0;
	int mhalf = m / 2;
	for (int i = 0; i < m; i++)
	{
		if (i < mhalf)
		{
			verticalMirroringKey |= (playerKey & bitMaskesForVerticalSymmetry[i]) << (m - 1 - 2 * i);
		}
		else if (i > mhalf || (i >= mhalf && bitMaskesForVerticalSymmetry[i] % 2 == 0)) // In the case of N is odd the middle row is not changed
		{
			verticalMirroringKey |= (playerKey & bitMaskesForVerticalSymmetry[i]) >> (2 * i - m + 1);
		}
		else
			if (i == mhalf && m % 2 == 1)
			{
				//if the gameboard m is odd, the middle colum has to be added as well. The row is unchanged
				verticalMirroringKey |= playerKey & bitMaskesForVerticalSymmetry[i];
			}
	}
	return verticalMirroringKey;
}

int64_t PNSData::mirrorKeyDiagonal(int64_t playerKey)
{
	//Although there is a diagonal and an antidiagonal we only need to compute one, as the other can be created by mirroring the diagonal

	// The idea of the diagonal mirroring is as follows:
	// Each element is moved depending of it distance to the diagonal. The two X below both have a distance of one to the diagonal (here we only count the horizontal/vertical distance). 
	// If we now move these X by m -1 (or m if we take the dead row into account), both Xs are mirrored. 
	//The O now has to be moved by 2* (m-1) [2m] to get to the correct postion. This strategie can be expanded for bigger fields very easy
	//	O X -
	//  X - -
	//  - - -
	int64_t diagonalMirroringKey = bitMaskesForDiagonalSymmetryLeftToRight[0] & playerKey;
	for (int i = 1; i < m; i++)
	{
		diagonalMirroringKey |= (playerKey & bitMaskesForDiagonalSymmetryLeftToRight[i]) >> m * i;
	}
	for (int i = 1; i < n; i++)
	{
		diagonalMirroringKey |= (playerKey & bitMaskesForDiagonalSymmetryRightToLeft[i]) << m * i;
	}
	return diagonalMirroringKey;
}
