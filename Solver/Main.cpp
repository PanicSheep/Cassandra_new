#include "Args.h"
#include "ConfigFile.h"
#include "HashTablePVS.h"
#include "IoPattern.h"
#include "IoPuzzleCollection.h"
#include "LastFlipCounter.h"
#include "Path.h"
#include "Pattern.h"
#include "PositionGenerator.h"
#include "PVSearch.h"
#include "Search.h"
#include "Stability.h"
#include "Utility.h"
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
	// Use cases:
	// Solve all unsolved puzzles:			solver.exe -f e10.pos
	// Solve all puzzles but don't save:	solver.exe -f e10.pos -test
	// Solve all puzzles:					solver.exe -f e10.pos -force
	// Solve a given depth:					solver.exe -f e10.pos -d 0
	// Solve given depths:					solver.exe -f e10.pos -d 1-8 // TODO: Implement!

	const auto exe_folder = CPath(argv[0]).GetAbsoluteFolderPath();
	CArgs args;

	// Set defaults.
	args.Set("pattern_names", exe_folder + "pattern_names.ini");
	args.Set("pattern_use", exe_folder + "pattern_use.ini");
	args.Set("RAM", "1GB");
	args.Set("d", 99);
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

	// Get arguments
	const CPath pattern_names = args.Get("pattern_names").back();
	const CPath pattern_use = args.Get("pattern_use").back();
	const CPath filename = args.Get("f").back();
	const std::string RAM = args.Get("RAM").back();
	const std::size_t threads = std::stoll(args.Get("t").back());
	const std::string depth = args.Get("d").back();
	const bool test = args.Has("test");
	const bool force = args.Has("force");
	const bool print_each_puzzle = args.Has("v");
	
	std::cout << "Filename: " << filename.GetAbsoluteFilePath() << std::endl;
	std::cout << "Filename: " << pattern_names.GetAbsoluteFilePath() << std::endl;
	std::cout << "Filename: " << pattern_use.GetAbsoluteFilePath() << std::endl;
	std::cout << "RAM: " << RAM << std::endl;
	std::cout << "Threads: " << threads << std::endl;
	std::cout << "Depth: " << depth << std::endl;
	std::cout << "Run as Test: " << test << std::endl;
	std::cout << "Print each puzzle: " << print_each_puzzle << std::endl;
	
	std::unique_ptr<PuzzleVectorGuard> puzzles;
	if (test)
		puzzles = std::make_unique<PuzzleVectorGuard>(LoadPuzzles(filename));
	else
		puzzles = std::make_unique<AutoSavingPuzzleVector>(LoadPuzzles(filename), filename, std::chrono::seconds(300));

	std::shared_ptr<ILastFlipCounter> last_flip_counter = std::make_shared<CLastFlipCounter>();
	std::shared_ptr<IHashTable<CPosition, PvsInfo>> hash_table = std::make_shared<CHashTablePVS>(ParseBytes(RAM) / sizeof(TwoNode));
	std::shared_ptr<IStabilityAnalyzer> stability_analyzer = std::make_shared<CStabilityAnalyzer>();
	std::shared_ptr<IEvaluator> midgame_evaluator = Pattern::IO::CreateEnsemble(pattern_names, pattern_use);
	std::shared_ptr<Engine> engine = std::make_shared<Engine>(nullptr, last_flip_counter, hash_table, stability_analyzer, midgame_evaluator);

	std::vector<std::unique_ptr<Search>> searches;
	for (std::size_t i = 0; i < puzzles->size(); i++)
		searches.emplace_back(std::make_unique<PVSearch>(engine));

	if (print_each_puzzle)
	{
		std::cout << " # | depth|score|       time (s) |      nodes (N) |    N/s     " << std::endl;
		std::cout << "---+------+-----+----------------+----------------+------------" << std::endl;
	}

	const auto GlobalStartTime = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for schedule(static, 1) num_threads(threads)
	for (int64_t i = 0; i < static_cast<int64_t>(puzzles->size()); i++)
	{
		auto puzzle = puzzles->Get(i);
		std::unique_ptr<Search>& search = searches[i];

		if (!force && !test && puzzle->IsSolved())
			continue;

		engine->AdvanceDate();

		auto old_puzzle = puzzle->Clone();
		if (test || force)
			puzzle->Reset();

		auto startTime = std::chrono::high_resolution_clock::now();
		puzzle->Solve(*search);
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
						<< (test && (*old_puzzle == *puzzle) ? " " : "#")
						<< DoubleDigitSignedInt(puzzleScore->MaxScore())
						<< (test && (*old_puzzle == *puzzle) ? " " : "#") << "|"
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
						<< (test && (*old_puzzle == *puzzle) ? " " : "#")
						<< DoubleDigitSignedInt(puzzleScore->score)
						<< DoubleDigitSignedInt(dynamic_cast<CPuzzleScore*>(old_puzzle.get())->score)
						<< (test && (*old_puzzle == *puzzle) ? " " : "#") << "|"
						<< std::setw(16) << time_format(endTime - startTime) << "|"
						<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|";

					if (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() > 0)
						std::cout << std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
					std::cout << std::endl;
				}
			}
			else if (const auto puzzleScore = dynamic_cast<CPuzzleAllDepthScore*>(puzzle.get()))
			{

				#pragma omp critical
				{
					std::cout
						<< std::setw(3) << i << "|"
						<< std::setw(6) << puzzle->GetPosition().EmptyCount() << "|"
						<< (test && (*old_puzzle == *puzzle) ? " " : "#")
						<< DoubleDigitSignedInt(puzzleScore->score[puzzleScore->MaxSolvedDepth()])
						<< (test && (*old_puzzle == *puzzle) ? " " : "#") << "|"
						<< std::setw(16) << time_format(endTime - startTime) << "|"
						<< std::setw(16) << ThousandsSeparator(search->GetNodeCount()) << "|";

					if (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() > 0)
						std::cout << std::setw(12) << ThousandsSeparator(search->GetNodeCount() * 1'000'000 / std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
					std::cout << std::endl;
				}
			}
		}

		puzzles->Set(i, std::move(puzzle));
	}
	const auto endTime = std::chrono::high_resolution_clock::now();

	std::size_t NodeCounter = 0;
	for (auto& search : searches)
		NodeCounter += search->GetNodeCount();

	const double time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - GlobalStartTime).count();

	if (print_each_puzzle)
		std::cout << "---+------+-----+----------------+----------------+------------" << std::endl;

	std::cout << ThousandsSeparator(NodeCounter) << " nodes in " << time_format(endTime - GlobalStartTime)
		<< " (" << ThousandsSeparator(NodeCounter * 1000.0 / time_diff) << " N/s)" << std::endl;
	
	if (!test)
		SavePuzzles(puzzles->Release(), filename);
	return 0;
}
