#include "Args.h"
#include "BoardCollection.h"
#include "ConfigFile.h"
#include "Path.h"
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
	const bool print_each_board = args.Has("v");

	CConfigurations configs(config_file);
	
	// Get arguments with config file
	std::vector<std::string> pattern_names;
	if (configs.Has("active pattern"))
		pattern_names = split(configs.Get("active pattern"), " ");

	std::vector<std::shared_ptr<CPatternGroup>> pattern_collection;
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
	std::shared_ptr<IPattern> PatternEvaluator = std::make_shared<CPatternCollection>(pattern_collection);
	
	//std::cout << "Filename: " << filename.GetAbsoluteFilePath() << std::endl;
	//std::cout << "Config File: " << config_file.GetAbsoluteFilePath() << std::endl;
	//std::cout << "RAM: " << RAM << std::endl;
	//std::cout << "Threads: " << threads << std::endl;
	//std::cout << "Run as Test: " << test << std::endl;
	//std::cout << "Print each board: " << print_each_board << std::endl;

	std::unique_ptr<CBoardCollection> boards;
	if (test)
		boards = std::make_unique<CBoardCollection>(filename);
	else
		boards = std::make_unique<AutoSavingBoardCollection>(filename, std::chrono::seconds(300));

	std::shared_ptr<ILastFLipCounter> LastFlipCounter = nullptr; // TODO: Replace!
	std::shared_ptr<IHashTable<TwoNode, CPosition, PvsInfo>> HashTable = std::make_shared<CHashTablePVS>(ParseBytes(RAM) / sizeof(TwoNode));
	std::shared_ptr<IStabilityAnalyzer> StabilityAnalyzer = nullptr; // TODO: Replace!
	std::shared_ptr<Environment> env = std::make_shared<Environment>(LastFlipCounter, HashTable, StabilityAnalyzer, PatternEvaluator);

	std::vector<std::unique_ptr<Search>> searches;
	for (std::size_t i = 0; i < boards->size(); i++)
		searches.emplace_back(std::make_unique<PVSearch>(env));

	if (print_each_board)
	{
		std::cout << " # | depth|score|       time (s) |      nodes (N) |    N/s     " << std::endl;
		std::cout << "---+------+-----+----------------+----------------+------------" << std::endl;
	}

	const auto startTime = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for schedule(static, 1) num_threads(threads)
	for (int64_t i = 0; i < boards->size(); i++)
	{
		auto board = boards->Get(i);
		std::unique_ptr<Search>& search = searches[i];

		if (!test && board->IsSolved())
			continue;

		env->HashTable->AdvanceDate();

		auto startTime = std::chrono::high_resolution_clock::now();
		auto score = search->Eval(board->GetPosition());
		auto endTime = std::chrono::high_resolution_clock::now();

		if (print_each_board)
		{
			if (const auto BoardScore = dynamic_cast<CBoardAllMoveScore*>(board.get()))
			{
				#pragma omp critical
				{
					std::cout
						<< std::setw(3) << i << "|"
						<< std::setw(6) << board->GetPosition().EmptyCount() << "|"
						<< (test && score != BoardScore->MaxScore() ? "#" : " ") << DoubleDigitSignedInt(score) << (test && score != BoardScore->MaxScore() ? "#" : " ") << "|"
						<< std::setw(16) << time_format(endTime - startTime) << "|"
						<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|";

					if (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() > 0)
						std::cout << std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
					std::cout << std::endl;
				}
			}
			else if (const auto BoardScore = dynamic_cast<CBoardScore*>(board.get()))
			{
				#pragma omp critical
				{
					std::cout
						<< std::setw(3) << i << "|"
						<< std::setw(6) << board->GetPosition().EmptyCount() << "|"
						<< (test && score != BoardScore->score ? "#" : " ") << DoubleDigitSignedInt(score) << (test && score != BoardScore->score ? "#" : " ") << "|"
						<< std::setw(16) << time_format(endTime - startTime) << "|"
						<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|";

					if (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() > 0)
						std::cout << std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
						std::cout << std::endl;
				}
			}
		}

		if (const auto BoardScore = dynamic_cast<CBoardScore*>(board.get()))
			BoardScore->score = score;

		boards->Set(i, board);
	}
	const auto endTime = std::chrono::high_resolution_clock::now();

	std::size_t NodeCounter = 0;
	for (auto& search : searches)
		NodeCounter += search->GetNodeCount();

	const double time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	if (print_each_board)
		std::cout << "---+------+-----+----------------+----------------+------------" << std::endl;

	std::cout << ThousandsSeparator(NodeCounter) << " nodes in " << time_format(endTime - startTime)
		<< " (" << ThousandsSeparator(NodeCounter * 1000.0 / time_diff) << " N/s)" << std::endl;
	
	if (!test)
		boards->Save(filename);
	return 0;
}
