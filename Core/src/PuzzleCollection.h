#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <shared_mutex>

#include "Puzzles.h"

using PuzzleVector = std::vector<std::unique_ptr<CPuzzle>>;

// Provides thread save access to the puzzles.
class PuzzleVectorGuard
{
protected:
	PuzzleVector m_puzzles;
	mutable std::shared_mutex m_mtx;

public:
	PuzzleVectorGuard() = delete;
	PuzzleVectorGuard(const PuzzleVector&) = delete;
	PuzzleVectorGuard(PuzzleVector&&);
	virtual ~PuzzleVectorGuard() {}

	virtual PuzzleVector Release();

	void push_back(std::unique_ptr<CPuzzle>&&);
	std::size_t size() const;

	std::unique_ptr<CPuzzle> Get(std::size_t index) const;
	void Set(std::size_t index, std::unique_ptr<CPuzzle>&&);
};

PuzzleVector Merge(std::vector<PuzzleVector>&&);