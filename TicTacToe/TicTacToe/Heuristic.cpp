#include "stdafx.h"
#include "Heuristic.h"

using namespace std;


Heuristic::Heuristic(int newm, int newn, int newk)
{
	int heuristicFieldSize = (newm+1) * newn;
	heuristicBoard.resize(heuristicFieldSize);
	m = newm;
	n = newn;
	k = newk;

}
Heuristic::Heuristic()
{
	heuristicBoard.resize(3 * 3);
	m = 3;
	n = 3;
	k = 3;

}

void Heuristic::initializeHeuristic(int newm, int newn, int newk)
{
	int heuristicFieldSize = (newm + 1) * newn;
	heuristicBoard.resize(heuristicFieldSize);
	m = newm;
	n = newn;
	k = newk;

}

void Heuristic::makeEntryZero(int x)
{
	 heuristicBoard.at(x) =0;
}

void Heuristic::decreaseWinningSetCount(int normalPos, int decreaseBy)
{
		heuristicBoard.at(normalPos) -= decreaseBy;
}

void Heuristic::setPosition(int data, int normalPos)
{
	heuristicBoard.at(normalPos) = data;
}

int Heuristic::getPosition(int normalPos)
{
	return heuristicBoard.at(normalPos);
}



void Heuristic::initializeHeuristicField()
{
	// values will be inistalizes first horizontal, vertical and then in the two diagonal ways 
	// horizontal
	for (int j = 0; j < n; j++)
	{
		for (int i = 0; i < m - k + 1; i++) // The upper two loop iterate overall (horizintal) winning  sets
		{
			for (int l = i; l < i + k; l++) // Iterates over all elements of a winning set
			{
				heuristicBoard.at(j*(m+1)+l)++; // Increase all elements of the winning set
			}
		}
	}


	for (int j = 0; j < n- k + 1; j++)
	{
		for (int i = 0; i < m; i++) // The upper two loop iterate overall (vertical) winning  sets
		{
			for (int l = j; l < j + k; l++) // Iterates over all elements of a winning set
			{
				heuristicBoard.at(l * (m+1) + i)++; // Increase all elements of the winning set
			}
		}
	}
	//diagonals
	for (int j = 0; j < n - k + 1; j++) // The diagonal has both the restrition from the vertical and horizontal rows
	{
		for (int i = 0; i < m - k + 1; i++) // We iterate over the entrys in the bottom right corner which are both to the left and top far enoth.
		{
			for (int l = 0; l < k; l++)
			{
				heuristicBoard.at((j + l) * (m+1) + i + l)++;
			}
		}
	}
	// Antidiagonal
	for (int j = 0; j < n - k + 1; j++) 
	{
		for (int i = k-1; i < m; i++)
		{
			for (int l = 0; l < k; l++)
			{
			heuristicBoard.at((j + l) * (m+1) + i-l)++;

			}
		}
	}


}


// Only if the whole board consists of zeros there are no more moves
bool Heuristic::noMoreMoves()
{
	// Is the game a draw? If no new winning sets are possible this holds. This is check here
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (heuristicBoard.at(j * m + i) != 0)
			{
				return false;
			}
		}
	}
	return true;
}

void Heuristic::printHeuristicField()
{
	cout << "- -" << endl;
	for (int i = n-1; i >= 0; i--)
	{
		cout << " ";
		for (int j = 0; j < m+1; j++)
		{
			int message = heuristicBoard.at(i*(m+1)+j);
			cout <<message << "  ";
		}
		cout << endl;
	}
	cout << endl;
}




