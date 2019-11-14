
#include "stdafx.h"
#include <iostream>
#include <string>

#include "TranspositionTable.h"
#include "ComputeMove.h"
#include "ReverseMove.h"
#include <math.h>
#include "Test.h"
#include "PNSController.h"
#include "Board.h"


/*
1. Usage and General purpose
This program by Manuel Neuberger has the purpose to solve (m,n,k)-games. When the program is started, the user is asked to enter the m n and k value. The three numbers have to be separated by the spacebar.
Afterwards he is asked which algorithm shell be used to solve the game. m stands for the minimax while p stands for the proof-number search apprach. 

For further question please see the thesis or contakt the autor under manuel.neuberger@live.de

*/
using namespace std;


// This method initializes the moveStack by putting all moves of depth one on it.
void initialiseMoveStack(stack<ComputeMove>& moveStack, Board board)
{
	// Some moves can be expluded because of the symmetrie
	for (int j = 0; j<ceil(board.getM()/ 2.0f); j++) // Only the rounded up half length is used, as all the other move are symmetric. 
	{
		for (int i = 0; i< ceil(board.getN() / 2.0f); i++) //
		{
			if (board.isSquare) // If the game is square, even more moves are symmetric.
			{
				if (j >= i)
				{
					moveStack.push(ComputeMove(1, true, (int64_t)pow(2,j+i*(board.getM()+1)),j + i * (board.getM() + 1))); // The +1 to m is added as there is allways a dead row
				}
			}
			else
			{
				moveStack.push(ComputeMove(1, true, (int64_t)pow(2, j + i * (board.getM() + 1)), j + i * (board.getM() + 1))); // The +1 is added as there is allways a dead row
			}
		}
	}
}


// Initialises the winDepthArray. All entry are set to unclear
void initialiseBestAtDepth(vector<Board::isWon>& winDepthArray, int size)
{
	for (int i = 0; i < size; i++)
	{
		winDepthArray.at(i) = Board::isWon::unclear;
	}
}



// Initializes the environment for minimax
void initialise(Board &gameboard, stack<ComputeMove>& moveStack, vector<Board::isWon>& winDepthArray, int winFoundForPlayerAtDepthSize)
{
	initialiseMoveStack(moveStack, gameboard);
	initialiseBestAtDepth(winDepthArray, winFoundForPlayerAtDepthSize);
}

// Starts the depth first/minimax Algorithm 
void startDepthFirstSearch(int m, int n , int k)
{
	// Variables for enhancements
	unique_ptr <TranspositionTable> transpositionTable(new TranspositionTable(178956970)); // This line takes a while. (a few seconds in release mode)
	transpositionTable->transpositionTable.clear();
	// The transpostion table is initalized before the clock is started to make different version easier to compar.

	// Start the clock
	clock_t start, end;
	start = clock();

	// Initializes the variables which are needed for the program.
	unique_ptr < Board> gameBoard(new Board(m, n, k)); // The gameboard on which the game is played
	unique_ptr < stack<ComputeMove>> moveStack(new stack<ComputeMove>()); // This stack stores all the nodes which still have to be expanded and evaluated.
	unique_ptr < stack<ReverseMove>> reverseStack(new stack<ReverseMove>()); // This stack stores the perfomed moves. This way they can be reverted by calling the reversemove funktion on the top element.

	
	int winDepthArraySize = ((gameBoard->getM()) * (gameBoard->getN())) + 1; // +1 as depth 0 is the result of the game
	// The winDepthArray is the array which saves the outcomes of game. Basically minimax is done in this array
	unique_ptr < vector<Board::isWon>> winDepthArray(new vector<Board::isWon>(winDepthArraySize)); // stores currently best result for the player whos turn it is.
	
	// Initialises the enviroment
	initialise(*gameBoard, *moveStack, *winDepthArray, winDepthArraySize);

	

	// This while loop is the main part of the program, as long as there are moves on the moveStack, it takes the top node and expands it.  
	while (moveStack->size() > 0)
	{
		ComputeMove move = moveStack->top();
		moveStack->pop();
		move.computeMove(*reverseStack, *moveStack, *gameBoard, *winDepthArray, *transpositionTable);
	}

	// Computation complete. 
	string message = "";
	if (winDepthArray->at(0) == Board::isWon::won)
	{
		message = "Player1 has won the game.";
	}
	else
	{
		message = "Player1 has lost the game.";
	}
	cout << message << endl;

	// Stop the clock 
	end = clock();
	cout << "The Programm took " << ((end - start) * 1.0f) / CLOCKS_PER_SEC << " s to execute." << endl;
	cout << gameBoard->turnId << " moves were computed sofar." << endl;
	cout << gameBoard->transpositionTableHits << " times the transpositionsTable was used." << endl;
	}

// Start the PNS Algorithm
void startPNSearch(int m, int n, int k)
{
	
	PNSController controller= PNSController();
	controller.startPNS(m,n,k);
	
}

int main()
{
	// This part handles the input
	bool correctInput = false;
	int m, n, k;
	// Enter m, n, k
	while (!correctInput)
	{
		cout << "Enter m n k" << endl;
		cin >> m;
		cin.ignore();
		cin >> n;
		cin.ignore();
		cin >> k;

		if (m > 0 && n>0  && k>0&& (m + 1) * n<=64)
		{
			correctInput = true; 
		}
		else
		{
			cin.clear();
			cin.ignore(100, '\n');
			cout << "Wrong input formate." << endl;
			cout << "m,n,k musst all be greater then 0. Also it should hold that (m+1)*n <=64" << endl;
		}
	}
	// Here the algorithm is chosen
	correctInput = false;
	char alg;
	while (!correctInput)
	{
		cout << "Which algorithm shell be used? (p = proof number search/ m = minimax/ b = both)" << endl;
		cin >> alg;
		if (alg == 'p')
		{
			cout << "Proof number search has been started" << endl;
			startPNSearch(m, n, k);
			correctInput = true;
		}
		else if (alg == 'm')
		{
			cout << "Minimax has been started" << endl;
			startDepthFirstSearch(m, n, k);
			correctInput = true;
		}else
		if (alg =='b')
		{
			cout << "The game is solution will be computed seperatly with both algorithms." << endl;
			cout << "Proof number search has been started" << endl;
			startPNSearch(m, n, k);
			cout << "Minimax has been started" << endl;
			startDepthFirstSearch(m, n, k);
			
			correctInput = true;
		}
		else
		{
			cout << "Illegal value. Please try again" << endl;
		}

	}
	system("pause");
 	return 0;
};

