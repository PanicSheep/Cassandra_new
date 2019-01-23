#include "Args.h"
#include "AlphaBetaFailHardSearch.h"
#include "AlphaBetaFailSoftSearch.h"
#include "Config.h"
#include "HashTablePVS.h"
#include "IoPattern.h"
#include "IoPuzzleCollection.h"
#include "LastFlipCounter.h"
#include "NegaMaxSearch.h"
#include "PVSearch.h"
#include "Path.h"
#include "Pattern.h"
#include "PositionGenerator.h"
#include "Search.h"
#include "Solver.h"
#include "Stability.h"
#include "Utility.h"
#include "VectorIO.h"
#include "VerboseCliArchive.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <omp.h>
#include <thread>
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

	Search::CSpecification spec(input.depth, 0);
	auto archive = Search::CreateArchive(input.test, input.print_each_puzzle, std::cout);

	CSolver solver(puzzles, spec, input.threads, input.test, input.force, input.print_each_puzzle);

	archive->Header();
	solver.Solve(Search::PVSearch(engine), *archive);
	archive->Footer();
	
	if (!input.test)
		SavePuzzles(puzzles->Release(), input.filename);

	hash_table->PrintStatistics();
	return 0;
}

