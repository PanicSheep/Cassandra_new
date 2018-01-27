#include "configfile.h"
#include "game.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <numeric>
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
	CPath filename = "C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\pos\\fforum-1-19.obf";												// Name of file to process
	std::size_t n = std::numeric_limits<std::size_t>::max();	// Number of positions to process
	std::size_t t = std::thread::hardware_concurrency();		// Number of threads to use
	int selectivity = NO_SELECTIVITY;
	unsigned int Depth = 99;
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
		else if (std::string(argv[i]) == "-d") Depth = atoi(argv[++i]);
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
	
	auto Positions = LoadVector(filename);

	std::vector<std::size_t> UnsovedIndex;
	if (bSkipSolved) {
		for (std::size_t i = 0; i < Positions.size(); i++)
		{
			if (Positions[i]->IsSolved())
				continue;
			else
				UnsovedIndex.push_back(i);
		}
	}
	else {
		UnsovedIndex.reserve(Positions.size());
		std::iota(UnsovedIndex.begin(), UnsovedIndex.end(), 0);
	}

	auto startTime = std::chrono::high_resolution_clock::now();
	std::size_t superNC = 0;
	//#pragma omp parallel for schedule(static,1) reduction(+:superNC)
	for (int64_t i = 0; i < static_cast<int64_t>(std::min(UnsovedIndex.size(), n)); i++)
	{
		auto GenericPos = Positions[UnsovedIndex[i]].get();
		uint64_t NC = 0;
		switch (GenericPos->ClassId())
		{
			case CPosition::classId:
			{
				auto pos = dynamic_cast<CPosition*>(GenericPos);
				auto depth = std::min(pos->EmptyCount(), static_cast<uint64_t>(Depth));
				auto startTime = std::chrono::high_resolution_clock::now();
				auto score = Eval(pos->P, pos->O, NC, selectivity, depth);
				auto endTime = std::chrono::high_resolution_clock::now();
				std::cout << SignedInt(score) << " " << ThousandsSeparator(NC) << "\t\t(" << ThousandsSeparator(NC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
				break;
			}
			case CPositionScore::classId:
			{
				auto pos = dynamic_cast<CPositionScore*>(GenericPos);
				auto depth = std::min(pos->EmptyCount(), static_cast<uint64_t>(Depth));
				pos->score = Eval(pos->P, pos->O, NC, selectivity, depth);
				break;
			}
			case CPositionScoreDepth::classId:
			{
				auto pos = dynamic_cast<CPositionScoreDepth*>(GenericPos);
				auto depth = std::min(pos->EmptyCount(), static_cast<uint64_t>(Depth));
				break;
			}
			case CPositionFullScore::classId:
			{
				auto pos = dynamic_cast<CPositionFullScore*>(GenericPos);
				auto depth = std::min(pos->EmptyCount(), static_cast<uint64_t>(Depth));
				for (unsigned int d = 0; d <= depth; d++)
					pos->score[d] = Eval(pos->P, pos->O, NC, selectivity, d);
				break;
			}
			case CPositionAllScore::classId:
			{
				auto pos = dynamic_cast<CPositionAllScore*>(GenericPos);
				auto depth = std::min(pos->EmptyCount(), static_cast<uint64_t>(Depth));
				auto startTime = std::chrono::high_resolution_clock::now();
				auto score = Eval(pos->P, pos->O, NC, selectivity, depth);
				auto endTime = std::chrono::high_resolution_clock::now();
				if (score == pos->MaxScore())
					std::cout << SignedInt(score) << " " << ThousandsSeparator(NC) << "\t\t(" << ThousandsSeparator(NC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
				else
					std::cout << "Wrong value! " << SignedInt(score) << " " << ThousandsSeparator(NC) << "\t\t(" << ThousandsSeparator(NC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
				break;
			}
			default:
				throw std::runtime_error("Invalid class name");
		}
		superNC += NC;
	}
	auto endTime = std::chrono::high_resolution_clock::now();

	if (bSave)
		Save(filename, Positions);

	std::cout << ThousandsSeparator(superNC) << " in " << time_format(endTime - startTime) << "\t\t(" << ThousandsSeparator(superNC * 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()) << " N/s)" << std::endl;
		
	return 0;
}
