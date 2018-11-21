#include "Args.h"
#include "ConfigFile.h" // TODO: Remove?
#include "Config.h"
#include "HashTablePVS.h"
#include "IoPattern.h"
#include "IoPuzzleCollection.h"
#include "LastFlipCounter.h"
#include "Path.h"
#include "Pattern.h"
#include "PositionGenerator.h"
#include "Search.h"
#include "NegaMaxSearch.h"
#include "AlphaBetaFailHardSearch.h"
#include "AlphaBetaFailSoftSearch.h"
#include "PVSearch.h"
#include "Stability.h"
#include "Utility.h"
#include "VectorIO.h"
#include "VerboseCliArchive.h"
#include "Solver.h"

#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <iterator>
#include <type_traits>
#include <memory>
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
	CArgs args(argc, argv);

	if (args.Has("h")) {
		PrintHelp();
		return 0;
	}

	CInputParser input(exe_folder, args);
	std::cout << input << std::endl;

	// Puzzles
	std::shared_ptr<PuzzleVectorGuard> puzzles;
	if (input.test)
		puzzles = std::make_shared<PuzzleVectorGuard>(LoadPuzzles(input.filename));
	else
		puzzles = std::make_shared<AutoSavingPuzzleVector>(LoadPuzzles(input.filename), input.filename, std::chrono::seconds(300));

	// Engine
	auto last_flip_counter = std::make_shared<CLastFlipCounter>();
	auto hash_table = std::make_shared<CHashTablePVS>(input.RAM / sizeof(TwoNode));
	auto stability_analyzer = std::make_shared<CStabilityAnalyzer>();
	auto midgame_evaluator = Pattern::IO::CreateEnsemble(input.pattern_names, input.pattern_use);
	auto engine = std::make_shared<Engine>(nullptr, last_flip_counter, hash_table, stability_analyzer, midgame_evaluator);

	// Algorithm
	std::unique_ptr<Search::CAlgorithm> algorithm = std::make_unique<Search::PVSearch>(engine);
	Search::CSpecification spec(-64, 64, input.depth, 0);

	// Output
	std::unique_ptr<Search::oArchive> archive = Search::CreateArchive(input.test, input.print_each_puzzle, std::cout);

	// Log
	std::unique_ptr<Search::ILog> logger;
	if (input.threads == 1)
		logger = std::make_unique<Search::CLogPassThrough>(*archive);
	else
		logger = std::make_unique<Search::CLogCollector>();

	archive->Header();
	const auto start_time = std::chrono::high_resolution_clock::now();
	{
		const auto size = static_cast<int64_t>(puzzles->size());

		#pragma omp parallel for num_threads(input.threads) schedule(dynamic,1)
		for (int64_t i = 0; i < size; i++)
		{
			auto puzzle = puzzles->Get(i);

			if (!input.test)
			{
				if (input.force)
					puzzle->Reset();
				else
					if (puzzle->IsSolved())
						continue; // Skip solved.
			}

			auto log = logger->Clone();
			log->index = i;

			puzzle->Solve(*algorithm->Clone(), *log, spec);

			*archive << *log;

			if (!input.test)
				puzzles->Set(i, std::move(puzzle));
		}
	}
	const auto end_time = std::chrono::high_resolution_clock::now();
	const double time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	archive->Footer();
		
	if (!input.test)
		SavePuzzles(puzzles->Release(), input.filename);
	return 0;
}

