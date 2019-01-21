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
#include "Stability.h"
#include "LastFlipCounter.h"
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

int main(int argc, char* argv[])
{
	const std::size_t SEED = 65481265;
	std::vector<std::size_t> Positions_per_EmptyCount =
	{
		1'000'000,
		1'000'000,
		1'000'000,
		1'000'000,
		1'000'000,
		1'000'000,
		1'000'000,
		  500'000,
		  250'000,
		  100'000,
		   50'000,
		   25'000,
		   10'000,
			5'000,
			2'500,
			1'000,
			  500,
			  250,
			  100,
			  100,
			  100,
			  100
	};

	CPositionGenerator pos_gen(SEED);
	std::size_t threads = std::thread::hardware_concurrency();
	
	class ZeroPattern : public IEvaluator
	{
	public:
		float Eval(const CPosition&) const override { return 0; }
	};
	
	auto last_flip_counter = std::make_shared<CLastFlipCounter>();
	auto hash_table = std::make_shared<CHashTablePVS>(1'000'000);
	auto stability_analyzer = std::make_shared<CStabilityAnalyzer>();
	auto midgame_evaluator = std::make_shared<ZeroPattern>();
	auto engine = std::make_shared<Engine>(nullptr, last_flip_counter, hash_table, stability_analyzer, midgame_evaluator);

	auto algorithm = std::make_unique<Search::PVSearch>(engine);

	for (std::size_t EmptyCount = 0; EmptyCount < Positions_per_EmptyCount.size(); EmptyCount++)
	{
		const auto PosCount = Positions_per_EmptyCount[EmptyCount];
		const auto pos_set = pos_gen.GenerateRandomPositionSet(EmptyCount, PosCount);

		std::vector<CPuzzleScore> puzzles;
		puzzles.reserve(pos_set.size());
		std::transform(pos_set.begin(), pos_set.end(), std::back_inserter(puzzles), 
					   [](const CPosition& pos) { return CPuzzleScore(pos); });

		std::vector<double> pos_per_s_vec;
		for (int k = 0; k < 1; k++)
		{
			hash_table->Clear();
			std::size_t node_counter = 0;
			const auto startTime = std::chrono::high_resolution_clock::now();
			#pragma omp parallel for schedule(dynamic, puzzles.size() / 1024) num_threads(threads) reduction(+:node_counter)
			for (int64_t i = 0; i < static_cast<int64_t>(puzzles.size()); i++)
			{
				auto clone = algorithm->Clone();
				puzzles[i].Solve(*clone);
				node_counter += clone->NodeCount();
			}
			const auto endTime = std::chrono::high_resolution_clock::now();

			const auto pico_seconds = std::chrono::duration_cast<std::chrono::duration<long long, std::pico>>(endTime - startTime);
			const auto ps_per_pos = pico_seconds / puzzles.size();
			const auto ps_per_Node = pico_seconds / node_counter;
			std::cout
				<< std::setw(2) << EmptyCount << " Empties: "
				<< short_time_format(ps_per_pos) << "/pos, "
				<< std::setprecision(2) << std::scientific << ps_per_pos.count() / static_cast<double>(std::pico::den) << " s/pos, "
				<< ThousandsSeparator(1.0 / ps_per_pos.count() * std::pico::den) << " pos/s, "
				<< ThousandsSeparator(1.0 / ps_per_Node.count() * std::pico::den) << " N/s" << std::endl;
			//pos_per_s_vec.push_back(1.0 / ps_per_pos.count() * std::pico::den);
		}
		//std::cout 
		//	<< std::setw(2) << EmptyCount << " Empties: "
		//	<< "Avg: " << average(pos_per_s_vec) << "pos/s    "
		//	<< "Stddev: " << StandardDeviation(pos_per_s_vec) << "pos/s    "
		//	<< "Limit: " << average(pos_per_s_vec) + 2 * StandardDeviation(pos_per_s_vec) << "pos/s\n";
	}
	return 0;
}