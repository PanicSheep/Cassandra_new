#include "Solver.h"
#include <iostream>

void CSolver::Solve(const Search::CAlgorithm& prototype)
{
	// Log
	std::unique_ptr<Search::ILog> logger;
	if (m_threads == 1)
		logger = std::make_unique<Search::CLogPassThrough>(*m_archive);
	else
		logger = std::make_unique<Search::CLogCollector>();
	
	const auto size = static_cast<int64_t>(m_puzzles->size());
	std::size_t omp_node_counter = 0;

	m_archive->Header();
	#pragma omp parallel for num_threads(m_threads) schedule(dynamic,1) reduction(+:omp_node_counter)
	for (int64_t i = 0; i < size; i++)
	{
		auto puzzle = m_puzzles->Get(i);

		if (!m_test)
		{
			if (m_force)
				puzzle->Reset();
			else
				if (puzzle->IsSolved())
					continue; // Skip solved.
		}

		auto search = prototype.Clone();
		auto log = logger->Clone();
		log->index = i;

		puzzle->Solve(*search, *log, m_specification);
		omp_node_counter += search->NodeCount();

		*m_archive << *log;

		if (!m_test)
			m_puzzles->Set(i, std::move(puzzle));
	}
	m_node_counter = omp_node_counter;
	m_archive->Footer();
}

std::size_t CSolver::NodeCount() const
{
	return m_node_counter;
}