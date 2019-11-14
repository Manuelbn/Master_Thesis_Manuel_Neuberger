#pragma once

#include <unordered_map>
#include <iostream>

// This class represents the Heuristik, which move to pick. It is only relevant if there are multiple possible moves (if the next move is determined by zugzwang for example this will not be used).
// The Heuristikclass consistis of a vector of the same size as the gameboard. Each entry has a value assined. This values represents the number of winning sets the current entry could participate in. 
// Entry which containe a stone will be marked with 0. As soon as all positions are marked with 0 the game can be stoped.


// This struct saves of how many different locations a entry is part of

using namespace std;
class Heuristic
{
	// Konstruktor
	public: 
		Heuristic(int m, int n, int k);
		Heuristic();

	// Variables
		void initializeHeuristic(int m, int n, int k);// This function is called from the construktor
	std::vector<int> heuristicBoard;
	int m;
	int n;
	int k;


	void makeEntryZero(int pos);
	void decreaseWinningSetCount(int normalPos, int decreaseBy);
	void setPosition(int data, int normalPos);
	int getPosition(int normalPos);

	// Generelle Funktions
	void initializeHeuristicField(); // Inistializes the values for the empty gameboard
	bool noMoreMoves(); // Checks if the game is a draw
	void printHeuristicField();

};

