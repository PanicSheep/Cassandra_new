#include "generatepositions.h"
#include "datamanipulation.h"
#include "utility.h"
#include "iterativealgorithms.h"
#include "vector_extensions.h"
#include "configfile.h"
#include "game.h"
#include "statistics.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <omp.h>

void print_help()
{
}

int main(int argc, char* argv[])
{
	std::vector<int> scoresDepth, scoresExact, scoresDiff;

	// Initialize
	//------------------------------------
	Configfile::Initialize(argv[0]);
	CountLastFlip::Initialize();
	Stability::Initialize();
	Pattern::Initialize();
	gTT = CHashTable(32 * 1024 * 1024);
	gTTPV = CHashTable(1024 * 1024);
	//------------------------------------
	
	for (int nEmpties = 0; nEmpties < 10; nEmpties++)
	{
		auto positions = GenerateRandomPositions(100000, nEmpties);
		
		for (int depth = 0; depth <= nEmpties; depth++)
		{
			for (auto& pos : positions)
			{
				uint64_t NC = 0;
				auto scoreDepth = PVS::Eval(pos.P, pos.O, NC, NO_SELECTIVITY, depth);
				auto scoreExact = PVS::Eval(pos.P, pos.O);
				scoresDepth.push_back(scoreDepth);
				scoresExact.push_back(scoreExact);
				scoresDiff.push_back(scoreDepth - scoreExact);
			}
			std::cout << "StandardDeviation(Depth = " << depth << ", Empties = " << nEmpties << ") = " << StandardDeviation(scoresDiff) << std::endl;
		}
	}
	return 0;
}