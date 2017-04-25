#include "configfile.h"
#include "game.h"
#include <iostream>
#include <chrono>
#include <omp.h>

int main(int argc, char* argv[])
{
	// Initialize
	Configfile::Initialize(argv[0]);
	CountLastFlip::Initialize();
	Stability::Initialize();
	Pattern::Initialize(false, false);
	gTT = CHashTable(32 * 1024 * 1024);
	gTTPV = CHashTable(1024 * 1024);
	
	std::string filename;
	bool bTest = false;
	
	// Parse input
	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == "-f") filename = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-test") bTest = true;
	}
	
	// Check input
	if (HasValidFilenameExtension(filename) == false) {
		std::cerr << "ERROR: File has no valid filename extension!" << std::endl;
		return -1;
	}
	
	// Use input
	std::string filename_extension = filename.substr(filename.rfind(".") + 1);
	if (filename_extension == CPosition::FILENAME_EXTENSION)
	{
		auto vec = LoadVector<CPosition>(filename);
		
		std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		std::size_t superNC = 0;
		for (auto& pos : vec)
		{
			uint64_t NC = 0;
			std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
			auto score = Endgame_PVS::Eval(pos.P, pos.O, NC);
			std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
			std::cout << SignedInt(score) << " " << ThousandsSeparator(NC) << "\t\t(" << ThousandsSeparator(NC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
			superNC += NC;
		}
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		std::cout << ThousandsSeparator(superNC) << " in " << time_format(endTime - startTime) << "\t\t(" << ThousandsSeparator(superNC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
	}
	else if (filename_extension == CPositionScore::FILENAME_EXTENSION)
	{
		auto vec = LoadVector<CPositionScore>(filename);
		
		std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		std::size_t superNC = 0;
		#pragma omp parallel for reduction(+:superNC)
		for (std::size_t i = 0; i < vec.size(); i++)
		{
			uint64_t NC = 0;
			auto score = Endgame_PVS::Eval(vec[i].P, vec[i].O, NC);
			superNC += NC;
		}
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		std::cout << ThousandsSeparator(superNC) << " in " << time_format(endTime - startTime) << "\t\t(" << ThousandsSeparator(superNC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
	}
	else if (filename_extension == "obf")
	{
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
	}
	return 0;
}