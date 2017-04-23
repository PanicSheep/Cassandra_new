#include "configfile.h"
#include "game.h"
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
	// Initialize
	gConfigurations = CConfigurations(argv[0]);
	CountLastFlip::Initialize();
	Stability::Initialize();
	gTT = CHashTable(32 * 1024 * 1024);
	gTTPV = CHashTable(1024 * 1024);
	
	std::string filename;
	
	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == "-f") filename = std::string(argv[++i]);
	}
	
	auto vec = LoadVector<CPositionAllScore>(filename);
	
	std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	std::size_t superNC = 0;
	for (std::size_t i = 0; i < vec.size(); i++)
	{
		uint64_t NC = 0;
		std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		auto score = Endgame_PVS::Eval(vec[i].P, vec[i].O, NC);
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		std::cout << SignedInt(score) << " " << ThousandsSeparator(NC) << "\t\t(" << ThousandsSeparator(NC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
		superNC += NC;
	}
	std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	std::cout << ThousandsSeparator(superNC) << " in " << time_format(endTime - startTime) << "\t\t(" << ThousandsSeparator(superNC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
	
	return 0;
}