#include "PositionGenerator.h"

#include <algorithm>
#include <functional>
#include <atomic>
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
	const std::unordered_set<CPosition>& GetSet() const { return set; }
	      std::unordered_set<CPosition>  GetSet()       { return set; }
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

unsigned int CPositionGenerator::rnd()
{
	return distribution(rndEngine);
}

CPositionGenerator::CPositionGenerator()
	: CPositionGenerator(std::random_device{}())
{}

CPositionGenerator::CPositionGenerator(std::size_t seed)
	: rndEngine(seed)
	, distribution(std::uniform_int_distribution<unsigned int>(0, 64))
{}

CPosition CPositionGenerator::GenerateRandomPosition(uint8_t EmptiesCount)
{
	CPosition pos = CPosition::StartPosition();

	for (auto plies = pos.EmptyCount() - EmptiesCount; plies > 0; plies--)
	{
		CMoves moves = pos.PossibleMoves();
		if (moves.empty())
		{
			pos = pos.PlayPass();
			moves = pos.PossibleMoves();
			if (moves.empty())
				return GenerateRandomPosition(EmptiesCount); // Start again.
		}
		for (int i = rnd() % moves.size(); i >= 0; i--)
			moves.ExtractMove();
		pos = pos.Play(moves.ExtractMove());
	}

	return pos;
}

std::unordered_set<CPosition> CPositionGenerator::GenerateRandomPositionSet(uint8_t EmptiesCount, std::size_t size)
{
	ThreadSavePosSet PosSet;
	auto gen = [&] { while (PosSet.size() < size) PosSet.TryInsert(GenerateRandomPosition(EmptiesCount), size); };

	std::vector<std::thread> threads;
	for (std::size_t i = 0; i < std::thread::hardware_concurrency() - 1; i++)
		threads.push_back(std::thread(gen));
	gen();

	for (auto& it : threads)
		it.join();

	return PosSet.GetSet();
}

// Not taking symmetrie into account.
void GenAll(CPosition pos, std::unordered_set<CPosition>& pos_set, const uint8_t depth)
{
	if (depth == 0) {
		pos_set.insert(pos);
		return;
	}

	auto moves = pos.PossibleMoves();

	if (moves.empty())
	{
		pos = pos.PlayPass();
		if (pos.HasMoves())
			GenAll(pos, pos_set, depth);
		return;
	}

	while (!moves.empty())
	{
		const auto move = moves.ExtractMove();
		GenAll(pos.Play(move), pos_set, depth - 1);
	}
}

// Taking symmetrie into account.
void GenAllSym(CPosition pos, std::unordered_set<CPosition>& pos_set, const uint8_t depth)
{
	if (depth == 0) {
		pos.FlipToMin();
		pos_set.insert(pos);
		return;
	}

	auto moves = pos.PossibleMoves();

	if (moves.empty())
	{
		pos = pos.PlayPass();
		if (pos.HasMoves())
			GenAllSym(pos, pos_set, depth);
		return;
	}

	while (!moves.empty())
	{
		const auto move = moves.ExtractMove();
		GenAllSym(pos.Play(move), pos_set, depth - 1);
	}
}

std::unordered_set<CPosition> CPositionGenerator::GenerateAllPositions(uint8_t EmptiesCount)
{
	std::unordered_set<CPosition> positions;
	CPosition pos = CPosition::StartPosition();
	GenAll(pos, positions, static_cast<uint8_t>(pos.EmptyCount() - EmptiesCount));
	return positions;
}

std::unordered_set<CPosition> CPositionGenerator::GenerateAllPositionsSym(uint8_t EmptiesCount)
{
	std::unordered_set<CPosition> positions;
	CPosition pos = CPosition::StartPosition();
	GenAllSym(pos, positions, static_cast<uint8_t>(pos.EmptyCount() - EmptiesCount));
	return positions;
}
