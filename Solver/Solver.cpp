#include "Solver.h"

void CSolver::Solve(const Search::CAlgorithm& prototype)
{
	const auto size = static_cast<int64_t>(m_puzzles->size());
	std::size_t omp_node_counter = 0;

#pragma omp parallel for num_threads(m_threads) reduction(+:omp_node_counter)
	for (int64_t i = 0; i < size; i++)
	{
		auto original_puzzle = m_puzzles->Get(i);
		auto puzzle = original_puzzle->Clone();
		auto search = prototype.Clone();

		if (m_test || m_force)
			puzzle->Reset();
		else
			if (puzzle->IsSolved())
				continue; // Skip solved.

		puzzle->Solve(*search);

		omp_node_counter += search->NodeCount();
		if (!m_test)
			m_puzzles->Set(i, std::move(puzzle));
	}
	m_node_counter = omp_node_counter;
}

std::size_t CSolver::NodeCount() const
{
	return m_node_counter;
}