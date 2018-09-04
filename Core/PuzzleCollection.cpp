#include "PuzzleCollection.h"

PuzzleVectorGuard::PuzzleVectorGuard(PuzzleVector&& puzzles)
	: m_puzzles(std::move(puzzles))
{}

PuzzleVector PuzzleVectorGuard::Release()
{
	std::unique_lock<std::mutex> lock(m_mtx);
	return std::move(m_puzzles);
}

void PuzzleVectorGuard::push_back(std::unique_ptr<CPuzzle>&& puzzle)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	m_puzzles.push_back(std::move(puzzle));
}

std::size_t PuzzleVectorGuard::size() const
{
	std::unique_lock<std::mutex> lock(m_mtx);
	return m_puzzles.size();
}

std::unique_ptr<CPuzzle> PuzzleVectorGuard::Get(std::size_t index) const
{
	std::unique_lock<std::mutex> lock(m_mtx);
	return m_puzzles[index]->Clone();
}

void PuzzleVectorGuard::Set(std::size_t index, std::unique_ptr<CPuzzle>&& puzzle)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	m_puzzles[index] = std::move(puzzle);
}