#pragma once
#include "PNSNode.h"
#include <iostream>

#include <chrono>
class PNSController
{
public:
	// similar to an internal main
	void startPNS(int newm, int newn, int newk);
	int8_t m;
	int8_t n;
	int8_t k;
	PNSNode root;
	int64_t turnId=0;
	//  Finds the MPN
	void findAndExpandMostProvingNode(PNSData& data, PNSTranspositionTable& tranpositionTable);
};

