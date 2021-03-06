#include "Path.h"
#include "Utility.h"
#include "HashTablePVS.h"
#include "PositionGenerator.h"
#include "Search.h"
#include "NegaMaxSearch.h"
#include "AlphaBetaFailHardSearch.h"
#include "AlphaBetaFailSoftSearch.h"
#include "PVSearch.h"
#include "Puzzles.h"
#include "Pattern.h"
#include "IoPattern.h"
#include "Stability.h"
#include "Solver.h"
#include "LastFlipCounter.h"
#include "PuzzleCollection.h"
#include "VerboseCliArchive.h"
#include <algorithm>
#include <numeric>
#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <iterator>

template <typename T>
double average(T container)
{
	return std::accumulate(container.begin(), container.end(), 0.0) / container.size();
}

template <typename T>
double StandardDeviation(T container)
{
	double sum = 0;
	double avg = average(container);
	for (const auto& it : container)
		sum += (it - avg) * (it - avg);
	return std::sqrt(sum / container.size());
}

PuzzleVector CreatePuzzles(std::size_t empty_count)
{
	std::vector<std::size_t> positions_per_empty_count =
	{
		1'000'000, // 0
		1'000'000,
		1'000'000,
		1'000'000,
		1'000'000,
		1'000'000, // 5
		1'000'000,
		  500'000,
		  250'000,
		  100'000,
		   50'000, // 10
		   25'000,
		   10'000,
			5'000,
			2'500,
			1'000, // 15
			  500,
			  250,
			  100,
			  100,
			  100, // 20
			  100,
			  100,
			  100,
			  100,
			  100  // 25
	};

	const std::size_t SEED = 65481265;
	CPositionGenerator pos_gen(SEED);

	PuzzleVector puzzles;
	const auto pos_set = pos_gen.GenerateRandomPositionSet(empty_count, positions_per_empty_count[empty_count]);
	std::transform(pos_set.begin(), pos_set.end(), std::back_inserter(puzzles),
		[](const CPosition& pos) { return std::make_unique<CPuzzleScore>(pos); });
	return puzzles;
}

PuzzleVector CreatePuzzles(std::size_t lower_empty_count, std::size_t upper_empty_count)
{
	PuzzleVector puzzles;
	for (std::size_t empty_count = lower_empty_count; empty_count <= upper_empty_count; empty_count++)
	{
		auto tmp = CreatePuzzles(empty_count);
		std::move(tmp.begin(), tmp.end(), std::back_inserter(puzzles));
	}
	return puzzles;
}

int main(int argc, char* argv[])
{
	const auto exe_folder = CPath(argv[0]).GetAbsoluteFolderPath();
	CPath pattern_names = exe_folder + "pattern_names.ini";
	CPath pattern_use = exe_folder + "pattern_use.ini";
	std::size_t RAM = 1ui64 << 30; // 1GB
	std::size_t threads = std::thread::hardware_concurrency();
	int8_t depth = 99;
	uint8_t selectivity = 0;
	bool test = true;
	bool force = true;
	bool print_each_puzzle = false;

	// Engine
	auto last_flip_counter = std::make_shared<CLastFlipCounter>();
	auto hash_table = std::make_shared<CHashTablePVS>(RAM / sizeof(CHashTablePVS::nodetype));
	auto stability_analyzer = std::make_shared<CStabilityAnalyzer>();
	auto midgame_evaluator = Pattern::IO::CreateEnsemble(pattern_names, pattern_use);
	auto engine = std::make_shared<Engine>(nullptr, last_flip_counter, hash_table, stability_analyzer, midgame_evaluator);

	Search::CSpecification spec(depth, selectivity);

	for (std::size_t empty_count = 15; empty_count <= 30; empty_count++)
	{
		// Puzzles
		auto puzzles = std::make_shared<PuzzleVectorGuard>(CreatePuzzles(empty_count));

		auto archive = Search::CreateArchive(test, print_each_puzzle, std::cout);

		CSolver solver(puzzles, spec, threads, test, force, print_each_puzzle);
		auto search = Search::PVSearch(engine);

		const auto startTime = std::chrono::high_resolution_clock::now();
		solver.Solve(search, *archive);
		const auto endTime = std::chrono::high_resolution_clock::now();

		//std::locale locale("");
		//std::cout.imbue(locale);
		//std::cout << empty_count << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms" << std::endl;
		//archive->Footer();
		const auto pico_seconds = std::chrono::duration_cast<std::chrono::duration<long long, std::pico>>(endTime - startTime);
		const auto ps_per_pos = pico_seconds / puzzles->size();
		const auto ps_per_Node = pico_seconds / solver.NodeCount();
		std::cout << std::setprecision(2) << std::scientific << ps_per_pos.count() / static_cast<double>(std::pico::den) << " ";
		//std::cout
		//	<< empty_count << " Empties: "
		//	<< short_time_format(ps_per_pos) << "/pos, "
		//	<< std::setprecision(2) << std::scientific << ps_per_pos.count() / static_cast<double>(std::pico::den) << " s/pos, "
		//	<< ThousandsSeparator(1.0 / ps_per_pos.count() * std::pico::den) << " pos/s, "
		//	<< ThousandsSeparator(1.0 / ps_per_Node.count() * std::pico::den) << " N/s" << std::endl;
	}
	hash_table->PrintStatistics();
	return 0;
}