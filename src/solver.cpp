#include "configfile.h"
#include "game.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <omp.h>

std::size_t ParseRAM(const std::string& s)
{
	if (s.find("EB") != std::string::npos) return std::stoll(s) * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
	if (s.find("PB") != std::string::npos) return std::stoll(s) * 1024 * 1024 * 1024 * 1024 * 1024;
	if (s.find("TB") != std::string::npos) return std::stoll(s) * 1024 * 1024 * 1024 * 1024;
	if (s.find("GB") != std::string::npos) return std::stoll(s) * 1024 * 1024 * 1024;
	if (s.find("MB") != std::string::npos) return std::stoll(s) * 1024 * 1024;
	if (s.find("kB") != std::string::npos) return std::stoll(s) * 1024;
	if (s.find("B")  != std::string::npos) return std::stoll(s);
	return 0;
}

void PrintHelp()
{
	std::cout << "Solves positions in a given file.\n"
		<< "-f	      filename\n"
		<< "-n        number of files to process\n"
		<< "-t        nubmer of threads to use\n"
		<< "-d        depth\n"
		<< "-ram      amount of ram to use\n"
		<< "-noskip   prevents skipping of solved positions\n"
		<< "-nosave   prevents saving of results\n"
		<< "-test     runs in test mode\n"
		<< "-h        prints this help." << std::endl;
}

int main(int argc, char* argv[])
{	
	std::string filename;										// Name of file to process
	std::size_t n = std::numeric_limits<std::size_t>::max();	// Number of positions to process
	std::size_t t = std::thread::hardware_concurrency();		// Number of threads to use
	int selectivity = NO_SELECTIVITY;
	unsigned int depth = 99;
	std::string RAM = "1GB";									// Amount of RAM to use for hashtable
	bool bTest = false;											// This run is a test
	bool bSkipSolved = true;									// Skip positions already solved
	bool bSave = true;											// Save results
	int verbosity = 0;											// Verbosity of output
	
	// Parse input
	//------------------------------------
	for (int i = 0; i < argc; i++)
	{
		     if (std::string(argv[i]) == "-f") filename = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-n") n = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-t") t = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-d") depth = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-RAM") RAM = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-noskip") bSkipSolved = false;
		else if (std::string(argv[i]) == "-nosave") bSave = false;
		else if (std::string(argv[i]) == "-test") { bSkipSolved = false; bSave = false; bTest = true; }
		else if (std::string(argv[i]) == "-verbose") verbosity = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-h") { PrintHelp(); return 0; }
	}
	//------------------------------------
	
	// Initialize
	//------------------------------------
	Configfile::Initialize(argv[0]);
	CountLastFlip::Initialize();
	Stability::Initialize();
	Pattern::Initialize();
	const std::size_t buckets = ParseRAM(RAM) / sizeof(CHashTable::NodeType);
	gTT = CHashTable(buckets);
	gTTPV = CHashTable(buckets / 32u);
	//------------------------------------
	
	// Check input
	//------------------------------------
	if (HasValidFilenameExtension(filename) == false) {
		std::cerr << "ERROR: File has no valid filename extension!" << std::endl;
		return -1;
	}
	//------------------------------------
	
	// Use input
	std::string filename_extension = filename.substr(filename.rfind(".") + 1);
	if (filename_extension == CPosition::FILENAME_EXTENSION)
	{	// CPosition
		auto vec = LoadVector<CPosition>(filename);
		
		std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		std::size_t superNC = 0;
		for (auto& pos : vec)
		{
			uint64_t NC = 0;
			std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
			auto score = Eval(pos.P, pos.O, NC, selectivity, depth);
			std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
			std::cout << SignedInt(score) << " " << ThousandsSeparator(NC) << "\t\t(" << ThousandsSeparator(NC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
			superNC += NC;
		}
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		std::cout << ThousandsSeparator(superNC) << " in " << time_format(endTime - startTime) << "\t\t(" << ThousandsSeparator(superNC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
	}
	else if (filename_extension == CPositionScore::FILENAME_EXTENSION)
	{	// CPositionScore
		auto vec = LoadVector<CPositionScore>(filename);
		
		std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		std::size_t superNC = 0;
		int64_t j = 0;
		for (; j < static_cast<int64_t>(vec.size()); j++)
			if (!(bSkipSolved && vec[j].IsSolved()))
				break;
		#pragma omp parallel for schedule(static,1) reduction(+:superNC)
		for (int64_t i = j; i < static_cast<int64_t>(MIN(vec.size(), j + n)); i++)
		{
			uint64_t NC = 0;
			if (!(bSkipSolved && vec[i].IsSolved()))
				vec[i].score = Eval(vec[i].P, vec[i].O, NC, selectivity, MIN(vec[i].EmptyCount(), depth));
			superNC += NC;
		}
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		
		if (bSave) SaveVector(filename, vec);
		std::cout << ThousandsSeparator(superNC) << " in " << time_format(endTime - startTime) << "\t\t(" << ThousandsSeparator(superNC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
	}
	else if (filename_extension == "obf")
	{	// CPositionAllScore
		auto vec = LoadVector<CPositionAllScore>(filename);

		std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
		std::size_t superNC = 0;
		for (std::size_t i = 0; i < vec.size(); i++)
		{
			uint64_t NC = 0;
			std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
			auto score = Eval(vec[i].P, vec[i].O, NC, selectivity, MIN(vec[i].EmptyCount(), depth));
			std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
			if (score == vec[i].MaxScore())
				std::cout << SignedInt(score) << " " << ThousandsSeparator(NC) << "\t\t(" << ThousandsSeparator(NC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
			else
				std::cout << "Wrong value! " << SignedInt(score) << " " << ThousandsSeparator(NC) << "\t\t(" << ThousandsSeparator(NC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
			superNC += NC;
		}
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		std::cout << ThousandsSeparator(superNC) << " in " << time_format(endTime - startTime) << "\t\t(" << ThousandsSeparator(superNC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
	}
	
	gTT.print_stats();
	gTTPV.print_stats();
	return 0;
}
