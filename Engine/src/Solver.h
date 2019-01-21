#pragma once
#include "PuzzleCollection.h"
#include <memory>
#include <utility>
#include <utility>

class CSolver
{
	std::shared_ptr<PuzzleVectorGuard> m_puzzles;
	Search::CSpecification m_specification;
	std::unique_ptr<Search::oArchive> m_archive;
	std::size_t m_node_counter;
	std::size_t m_threads;
	bool m_test;
	bool m_force;
	bool m_print_each_puzzle;

public:
	CSolver(std::shared_ptr<PuzzleVectorGuard> puzzles,
		Search::CSpecification specification,
		std::unique_ptr<Search::oArchive>&& archive,
		std::size_t threads,
		bool test, bool force, bool print_each_puzzle)
		: m_puzzles(std::move(puzzles))
		, m_specification(specification)
		, m_archive(std::move(archive))
		, m_node_counter(0)
		, m_threads(threads)
		, m_test(test)
		, m_force(force)
		, m_print_each_puzzle(print_each_puzzle)
	{}

	void Solve(const Search::CAlgorithm&);
	std::size_t NodeCount() const;
};
