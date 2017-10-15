#include "generatepositions.h"
#include <thread>
#include <mutex>
#include <omp.h>

class ThreadSavePosSet
{
	mutable std::mutex mtx;
	std::unordered_set<CPosition> set;
public:
	bool TryInsert(const CPosition& pos, std::size_t maxSize);
	std::size_t size() const;
	inline const std::unordered_set<CPosition>& GetSet() const { return set; }
	inline       std::unordered_set<CPosition>  GetSet()       { return set; }
};

bool ThreadSavePosSet::TryInsert(const CPosition& pos, std::size_t maxSize)
{
	std::lock_guard<std::mutex> guard(mtx);
	if (set.size() >= maxSize)
		return false;
	const bool InsertionTookPlace = set.insert(pos).second;
	return InsertionTookPlace;
}

std::size_t ThreadSavePosSet::size() const
{
	std::lock_guard<std::mutex> guard(mtx);
	return set.size();
}

void GenerateRandomPositionsThreadFunction(std::size_t SetSize, const uint8_t numEmpties, const bool ETH, ThreadSavePosSet& set, std::size_t seed)
{
	auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0, 64), std::mt19937_64(seed));

	while (set.size() < SetSize)
	{
		CPosition pos(ETH);
		unsigned int plies = pos.EmptyCount() - numEmpties;
		while (plies > 0)
		{
			uint64_t possibles = pos.PossibleMoves();
			if (!possibles)
			{
				pos.PlayStone(64);
				possibles = pos.PossibleMoves();
				if (!possibles)
				{
					pos.Reset(ETH);
					plies = pos.EmptyCount() - numEmpties;
					continue;
				}
			}
			pos.PlayStone(BitScanLSB(PDep(1ULL << (rnd() % PopCount(possibles)), possibles)));
			plies--;
		}
		set.TryInsert(pos, SetSize);
	}
}


// Non deterministic but fast
std::unordered_set<CPosition> GenerateRandomPositions(const std::size_t numPos, const uint8_t numEmpties, const bool ETH)
{
	ThreadSavePosSet set;
	std::size_t seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::vector<std::thread> threads;

	for (std::size_t i = 0; i < std::thread::hardware_concurrency() - 1; i++)
		threads.push_back(std::thread(GenerateRandomPositionsThreadFunction, numPos, numEmpties, ETH, std::ref(set), seed++));
	GenerateRandomPositionsThreadFunction(numPos, numEmpties, ETH, set, seed++);

	for (auto& it : threads)
		it.join();

	return set.GetSet();
}

// Deterministic
std::unordered_set<CPosition> GenerateRandomPositions(const std::size_t numPos, const uint8_t numEmpties, const bool ETH, const std::size_t seedNum)
{
	std::unordered_set<CPosition> pos_set;
	std::mutex pos_set_mutex;
	std::atomic<std::size_t> seed;
	seed.store(seedNum);
	
	while (pos_set.size() < numPos)
	{
		const std::size_t pos_set_size = pos_set.size();
		#pragma omp parallel
		{
			auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0, 64), std::mt19937_64(seed.fetch_add(1)));

			#pragma omp for
			for (int64_t i = pos_set_size; i < static_cast<int64_t>(numPos); i++)
			{
				CPosition pos(ETH);
				unsigned int plies = pos.EmptyCount() - numEmpties;
				while (plies > 0)
				{
					uint64_t possibles = pos.PossibleMoves();
					if (!possibles)
					{
						pos.PlayStone(64);
						possibles = pos.PossibleMoves();
						if (!possibles)
						{
							pos.Reset(ETH);
							plies = pos.EmptyCount() - numEmpties;
							continue;
						}
					}
					pos.PlayStone(BitScanLSB(PDep(1ULL << (rnd() % PopCount(possibles)), possibles)));
					plies--;
				}
				{
					std::lock_guard<std::mutex> guard(pos_set_mutex);
					pos_set.insert(pos);
				}
			}
		}
	}
	
	return pos_set;
}

void GenAll(CPosition pos, std::unordered_set<CPosition>& pos_set, const unsigned int depth)
{	
	if (depth == 0) {
		pos.FlipToMin();
		pos_set.insert(pos);
		return;
	}

	uint64_t moves = pos.PossibleMoves();

	if (!moves)
	{
		pos.PlayStone(64);
		if (pos.HasMoves())
			GenAll(pos, pos_set, depth);
		return;
	}
	
	while (moves)
	{
		const auto move = BitScanLSB(moves);
		RemoveLSB(moves);
		CPosition next = pos;
		next.PlayStone(move);
		GenAll(next, pos_set, depth-1);
	}
}

std::unordered_set<CPosition> GenerateAllPositions(const uint8_t numEmpties, const bool ETH)
{
	std::unordered_set<CPosition> pos_set;
	
	CPosition pos(ETH);
	GenAll(pos, pos_set, pos.EmptyCount() - numEmpties);
	return pos_set;
}
