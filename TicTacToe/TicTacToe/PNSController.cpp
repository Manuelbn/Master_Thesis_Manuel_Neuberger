#include "stdafx.h"
#include "PNSController.h"

void PNSController::startPNS(int newm, int newn, int newk)
{
	// As the createn of the transposition table can take a while we do this befor starting the clock
	unique_ptr<PNSTranspositionTable> transpositionTable{ new PNSTranspositionTable(178956970) };
	// Start the clock
	clock_t start, end;

	start = clock();
	m = newm;
	n = newn;
	k = newk;

	//Values for RootNode
	int64_t playerOnBoard = 0;
	int64_t playerTwoBoard = 0;

	// Both gameboard are 0 (as they are emtpy), there is no parent to the root so it is NULL, as there was not last move it is 0 , the depth is -1 as it will be increased in the construktor it is 0

	unique_ptr<PNSData> data{ new PNSData(m,n,k) };
	
	
	root = PNSNode(0, 0, true, -1); // the board of P1 and P2 are both emtyp =0, as P1 has the first move we assign the root to p2
	root.expandNode(*data, *transpositionTable);

	//findAndExpendMostProvingNode(*data);
	turnId =0;
	while (!root.isSolved) // As long the root is not solved new nodes will be expanded
	{
		//cout << turnId << endl;
		findAndExpandMostProvingNode(*data, *transpositionTable);
		turnId++;
		if (turnId % 1000000 == 0 && turnId != 0)
		{
			cout << "I am still working at move " << turnId << endl;
		}
	}
	if (root.proofNumber == 0)
	{
		cout << "Player 1 has won the game" << endl;
	}
	else
	{
		cout << "The game has ended in a draw" << endl;
	}
	cout << "It took " << turnId << " moves." << endl;
	// Stop the clock 
	end = clock();
	cout << "The Programm took " << ((end - start) * 1.0f) / CLOCKS_PER_SEC << " s to execute." << endl;
}


void PNSController::findAndExpandMostProvingNode(PNSData& data, PNSTranspositionTable& tranpositionTable)
{
	// This function finds the MPN and expands the node. For this we start from the root and search for the MPN path at all depths until a node is chossen which has not yet been expanded.

 	bool MPNfound = false;
	int64_t importendMinimal=0; // if it is P1 turn all children (which aren´t solved) are part of the disproof set. if it is P2 turn it is reversed. This means we only have to look for the other numbers
	PNSNode* currentNode = &root;
	while (!MPNfound)
	{
		int i = 0; // we basiclly have a for loop here which in devided in to 2 parts:
		// Part 1: Pics the first element. Most of the time this will be just iterated over onces.
		// this loop part is basiclly just pic the first element, but if the first element is allready solved pick another element. There allways has to be a unsolved element as otherwise the parent would be solved
		for (; i < currentNode->children.size(); i++) 
		{
			if (!currentNode->children.at(i).isSolved)
			{
				if (currentNode->currentPlayerisOne)
				{
					importendMinimal = currentNode->children.at(i).proofNumber;
				}
				else
				{
					importendMinimal = currentNode->children.at(i).disproofNumber;
				}
				i++; // as the i++ is not increased form the loop after a break we increase it here so we don´t do one check twice
				break;
			}
		}
		// Part 2
		// Find the proof and disproof number of the MPN
		for (; i < currentNode->children.size(); i++)
		{
			if (!currentNode->children.at(i).isSolved)
			{
				if (currentNode->currentPlayerisOne)
				{
					if (currentNode->children.at(i).proofNumber < importendMinimal)
					{
						importendMinimal = currentNode->children.at(i).proofNumber;
					}
				}
				else
				if (currentNode->children.at(i).disproofNumber < importendMinimal)
				{
					importendMinimal = currentNode->children.at(i).disproofNumber;
				}
			}
		}
		// Find the MPN
		for (int j = 0; j < currentNode->children.size(); j++)
		{ 
				// If it is the current players turn we only look at the proof number id not at the disproof nubmer.
				if ((importendMinimal == currentNode->children.at(j).proofNumber && currentNode->currentPlayerisOne)|| (importendMinimal == currentNode->children.at(j).disproofNumber&&!currentNode->currentPlayerisOne))
				{
					if (!currentNode->children.at(j).isExpanded)
					{
						MPNfound = true; // stops loop
						//cout << "TurnId " << turnId << ", depth " << (currentNode->depth + 0) << endl;
						currentNode->children.at(j).expandNode(data,tranpositionTable );
					}
					else
					{
						currentNode = &(currentNode->children.at(j));

					}
					break;
				}
			
		}
	}
}




