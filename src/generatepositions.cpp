#include "generatepositions.h"
#include <thread>
#include <mutex>
#include <omp.h>

std::unordered_set<CPosition> GenerateRandomPositions(const std::size_t numPos, const uint8_t numEmpties, const bool ETH)
{
	std::unordered_set<CPosition> pos_set;
	std::mutex pos_set_mutex;
	std::atomic<std::size_t> seed;
	seed.store(std::chrono::system_clock::now().time_since_epoch().count());
	
	#pragma omp parallel
	{
		auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0, 64), std::mt19937_64(seed.fetch_add(1)));
		
		#pragma omp for
		for (int64_t i = 0; i < numPos; i++)
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
				//for (unsigned int k = rnd() % PopCount(possibles); k > 0; k--)
				//	RemoveLSB(possibles);
				//pos.PlayStone(BitScanLSB(possibles));
				pos.PlayStone(BitScanLSB(PDep(1ULL << (rnd() % PopCount(possibles)), possibles)));
				plies--;
			}
			{
				std::lock_guard<std::mutex> guard(pos_set_mutex);
				pos_set.insert(pos);
			}
		}
	}
	
	auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0, 64), std::mt19937_64(seed.fetch_add(1)));
	while (pos_set.size() < numPos)
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
			//for (unsigned int k = rnd() % PopCount(possibles); k > 0; k--)
			//	RemoveLSB(possibles);
			//pos.PlayStone(BitScanLSB(possibles));
			pos.PlayStone(BitScanLSB(PDep(1ULL << (rnd() % PopCount(possibles)), possibles)));
			plies--;
		}
		pos_set.insert(pos);
	}
	return pos_set;
}

void GenAll(CPosition pos, std::unordered_set<CPosition>& pos_set, const unsigned int depth)
{
	pos.FlipToMin();
	
	if (pos_set.count(pos))
		return;
	
	if (depth == 0) {
		pos_set.insert(pos);
		return;
	}

	uint64_t possibles = pos.PossibleMoves();

	if (!possibles)
	{
		pos.PlayStone(64);
		if (pos.HasMoves())
			GenAll(pos, pos_set, depth);
		return;
	}
	
	while (possibles)
	{
		const auto move = BitScanLSB(possibles);
		RemoveLSB(possibles);
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
