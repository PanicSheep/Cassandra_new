#include "Args.h"
#include "IoPuzzleCollection.h"
#include "ConfigFile.h"
#include "HashTablePVS.h"
#include "LastFlipCounter.h"
#include "Path.h"
#include "PVSearch.h"
#include "Utility.h"
#include "Pattern.h"
#include "PositionGenerator.h"
#include "Search.h"
#include "Stability.h"
#include "VectorIO.h"
#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <iterator>
#include <type_traits>
#include <omp.h>

using namespace IO;

void PrintHelp()
{
	std::cout << "Expected input:\n"
		<< "-f filename\n"
		<< "-test\n";
}

int main(int argc, char* argv[])
{
	CPattern::Initialize();

	CArgs args;
	args.Set("config", "config.ini");
	args.Set("RAM", "1GB");
	args.Set("t", std::thread::hardware_concurrency());
	args.Load(argc, argv);

	if (args.Has("h")) {
		PrintHelp();
		return 0;
	}

	if (!args.Has("f")) {
		std::cerr << "ERROR: Missing filename!" << std::endl;
		return -1;
	}

	// Get arguments without config file
	const CPath config_file = args.Get("config")[0];
	const CPath filename = args.Get("f")[0];
	const std::string RAM = args.Get("RAM")[0];
	const std::size_t threads = std::stoll(args.Get("t")[0]);
	const bool test = args.Has("test");
	const bool print_each_puzzle = args.Has("v");

	CConfigurations configs(config_file);
	
	// Get arguments with config file
	std::vector<std::string> pattern_names;
	if (configs.Has("active pattern"))
		pattern_names = split(configs.Get("active pattern"), " ");

	std::vector<std::shared_ptr<CPatternGroup>> pattern_collection;
#ifndef _DEBUG
	for (int range = 0; range < 20; range++)
	{
		std::vector<std::shared_ptr<CPattern>> pattern_group;
		for (const auto& pat : pattern_names)
		{
			uint64_t pattern;
			if (configs.Has("pattern " + pat))
				pattern = std::stoull(configs.Get("pattern " + pat), nullptr, 16);
			else
				std::cerr << "WARNING: Unknown pattern '" << pat << "'." << std::endl;

			CPath weights(config_file.GetAbsoluteFolderPath() + configs.Get("weights" + std::to_string(range)) + pat + ".w");
			auto Pattern = CreatePattern(pattern);
			Pattern->set_weights(read_vector<float>(weights));
			pattern_group.push_back(std::move(Pattern));
		}
		auto shared_pattern_group = std::make_shared<CPatternGroup>(pattern_group);
		pattern_collection.push_back(shared_pattern_group);
		pattern_collection.push_back(shared_pattern_group);
		pattern_collection.push_back(shared_pattern_group);
		if (range == 0)
			pattern_collection.push_back(shared_pattern_group);
	}
#endif
	std::shared_ptr<IPattern> PatternEvaluator = std::make_shared<CPatternCollection>(pattern_collection);
	
	//std::cout << "Filename: " << filename.GetAbsoluteFilePath() << std::endl;
	//std::cout << "Config File: " << config_file.GetAbsoluteFilePath() << std::endl;
	//std::cout << "RAM: " << RAM << std::endl;
	//std::cout << "Threads: " << threads << std::endl;
	//std::cout << "Run as Test: " << test << std::endl;
	//std::cout << "Print each puzzle: " << print_each_puzzle << std::endl;
	
	std::unique_ptr<FilePuzzleCollection> puzzles;
	if (test)
		puzzles = std::make_unique<FilePuzzleCollection>(filename);
	else
		puzzles = std::make_unique<AutoSavingPuzzleCollection>(filename, std::chrono::seconds(300));

	std::shared_ptr<ILastFlipCounter> LastFlipCounter = std::make_shared<CLastFlipCounter>();
	std::shared_ptr<IHashTable<CPosition, PvsInfo>> HashTable = std::make_shared<CHashTablePVS>(ParseBytes(RAM) / sizeof(TwoNode));
	std::shared_ptr<IStabilityAnalyzer> StabilityAnalyzer = std::make_shared<CStabilityAnalyzer>();
	std::shared_ptr<Environment> env = std::make_shared<Environment>(nullptr, LastFlipCounter, HashTable, StabilityAnalyzer, PatternEvaluator);

	std::vector<std::unique_ptr<Search>> searches;
	for (std::size_t i = 0; i < puzzles->size(); i++)
		searches.emplace_back(std::make_unique<PVSearch>(env));

	if (print_each_puzzle)
	{
		std::cout << " # | depth|score|       time (s) |      nodes (N) |    N/s     " << std::endl;
		std::cout << "---+------+-----+----------------+----------------+------------" << std::endl;
	}

	const auto startTime = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for schedule(static, 1) num_threads(threads)
	for (int64_t i = 0; i < puzzles->size(); i++)
	{
		auto puzzle = puzzles->Get(i);
		std::unique_ptr<Search>& search = searches[i];

		if (!test && puzzle->IsSolved())
			continue;

		env->HashTable->AdvanceDate();

		auto startTime = std::chrono::high_resolution_clock::now();
		auto score = search->Eval(puzzle->GetPosition());
		auto endTime = std::chrono::high_resolution_clock::now();

		if (print_each_puzzle)
		{
			if (const auto puzzleScore = dynamic_cast<CPuzzleAllMoveScore*>(puzzle.get()))
			{
				#pragma omp critical
				{
					std::cout
						<< std::setw(3) << i << "|"
						<< std::setw(6) << puzzle->GetPosition().EmptyCount() << "|"
						<< (test && score != puzzleScore->MaxScore() ? "#" : " ") << DoubleDigitSignedInt(score) << (test && score != puzzleScore->MaxScore() ? "#" : " ") << "|"
						<< std::setw(16) << time_format(endTime - startTime) << "|"
						<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|";

					if (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() > 0)
						std::cout << std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
					std::cout << std::endl;
				}
			}
			else if (const auto puzzleScore = dynamic_cast<CPuzzleScore*>(puzzle.get()))
			{
				#pragma omp critical
				{
					std::cout
						<< std::setw(3) << i << "|"
						<< std::setw(6) << puzzle->GetPosition().EmptyCount() << "|"
						<< (test && score != puzzleScore->score ? "#" : " ") << DoubleDigitSignedInt(score) << (test && score != puzzleScore->score ? "#" : " ") << "|"
						<< std::setw(16) << time_format(endTime - startTime) << "|"
						<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|";

					if (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() > 0)
						std::cout << std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
						std::cout << std::endl;
				}
			}
		}

		if (const auto puzzleScore = dynamic_cast<CPuzzleScore*>(puzzle.get()))
			puzzleScore->score = score;

		puzzles->Set(i, std::move(puzzle));
	}
	const auto endTime = std::chrono::high_resolution_clock::now();

	std::size_t NodeCounter = 0;
	for (auto& search : searches)
		NodeCounter += search->GetNodeCount();

	const double time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	if (print_each_puzzle)
		std::cout << "---+------+-----+----------------+----------------+------------" << std::endl;

	std::cout << ThousandsSeparator(NodeCounter) << " nodes in " << time_format(endTime - startTime)
		<< " (" << ThousandsSeparator(NodeCounter * 1000.0 / time_diff) << " N/s)" << std::endl;
	
	if (!test)
		puzzles->Save(filename);
	return 0;
}
