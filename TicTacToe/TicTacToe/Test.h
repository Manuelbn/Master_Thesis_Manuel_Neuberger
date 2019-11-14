#pragma once

#include <iostream>
#include "TranspositionTable.h"
#include "Heuristic.h"
#include "ReverseMove.h"
class Test
{
	public:

	bool symmetrieHashTestOnSquare();

	bool symmetrieHashTestOnNotSquare();

	bool hashMethodTest();

	bool tableTest();

	void heuristicTest();
};

