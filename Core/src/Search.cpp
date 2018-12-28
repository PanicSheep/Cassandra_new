#include "Search.h"

#include <utility>

namespace Search
{
	CSpecification::CSpecification(int8_t depth, uint8_t selectivity)
		: depth(depth), selectivity(selectivity)
	{}

	CResult::CResult(int8_t score, std::size_t node_count, std::chrono::nanoseconds duration)
		: score(score), node_count(node_count), duration(duration)
	{}

	CAlgorithm::CAlgorithm(std::shared_ptr<Engine> engine)
		: engine(std::move(engine))
	{}

	CResult CAlgorithm::Eval(const CPosition& pos)
	{
		return Eval(pos, CSpecification::SolveExact(pos));
	}

	int CAlgorithm::EvalGameOver(const CPosition& pos)
	{
		const auto Ps = static_cast<int>(PopCount(pos.GetP()));
		const auto Os = static_cast<int>(PopCount(pos.GetO()));
		if (Ps > Os)
			return 64 - 2 * Os;
		if (Ps < Os)
			return 2 * Ps - 64;
		return Ps - Os;
	}

	std::unique_ptr<ILog> CLogNull::Clone() const
	{
		return std::make_unique<CLogNull>();
	}

	CLogCollector::CLogCollector(const CLogCollector& o) : ILog(o), m_vec(o.m_vec) {}

	std::unique_ptr<ILog> CLogCollector::Clone() const
	{
		return std::make_unique<CLogCollector>(*this);
	}

	CLogPassThrough::CLogPassThrough(const CLogPassThrough& o) : ILog(o), m_archive(o.m_archive) {}

	std::unique_ptr<ILog> CLogPassThrough::Clone() const
	{
		return std::make_unique<CLogPassThrough>(*this);
	}

	//ILog::ILog(CPosition position, int8_t depth, uint8_t selectivity,
	//		   int score, std::size_t node_count, std::chrono::nanoseconds duration)
	//	: position(position)
	//	, score(score)
	//	, depth(depth)
	//	, selectivity(selectivity)
	//	, node_count(node_count)
	//	, duration(duration)
	//{}

	//CTestLog::CTestLog(CPosition position, int8_t depth, uint8_t selectivity, int original_score,
	//				   int score, std::size_t node_count, std::chrono::nanoseconds duration)
	//	: ILog(position, depth, selectivity, score, node_count, duration)
	//	, original_score(original_score)
	//{}

	//CLogger::CLogger(std::unique_ptr<CAlgorithm>&& search)
	//	: CAlgorithm(*search)
	//	, m_search(std::move(search))
	//{}

	//int CLogger::Eval(const CPosition& pos)
	//{
	//	const auto old_node_counter = m_search->NodeCount();
	//	const int8_t depth = static_cast<int8_t>(pos.EmptyCount());
	//	const uint8_t selectivity = 0;
	//	const auto start_time = std::chrono::high_resolution_clock::now();

	//	const auto score = m_search->Eval(pos);

	//	const auto end_time = std::chrono::high_resolution_clock::now();
	//	const auto duration = end_time - start_time;
	//	const auto node_count = m_search->NodeCount() - old_node_counter;

	//	m_logs.emplace_back(std::make_unique<ILog>(pos, depth, selectivity, score, node_count, duration));
	//	return score;
	//}

	//int CLogger::Eval(const CPosition& pos, int8_t depth, uint8_t selectivity)
	//{
	//	const auto old_node_counter = m_search->NodeCount();
	//	const auto start_time = std::chrono::high_resolution_clock::now();

	//	const auto score = m_search->Eval(pos, depth, selectivity);

	//	const auto end_time = std::chrono::high_resolution_clock::now();
	//	const auto duration = end_time - start_time;
	//	const auto node_count = m_search->NodeCount() - old_node_counter;

	//	m_logs.emplace_back(std::make_unique<ILog>(pos, depth, selectivity, score, node_count, duration));
	//	return score;
	//}

	//int CLogger::TestEval(const CPosition& pos, int original_score)
	//{
	//	const auto old_node_counter = m_search->NodeCount();
	//	const int8_t depth = static_cast<int8_t>(pos.EmptyCount());
	//	const uint8_t selectivity = 0;
	//	const auto start_time = std::chrono::high_resolution_clock::now();

	//	const auto score = m_search->TestEval(pos, original_score);

	//	const auto end_time = std::chrono::high_resolution_clock::now();
	//	const auto duration = end_time - start_time;
	//	const auto node_count = m_search->NodeCount() - old_node_counter;

	//	m_logs.emplace_back(std::make_unique<CTestLog>(pos, depth, selectivity, original_score, score, node_count, duration));
	//	return score;
	//}

	//int CLogger::TestEval(const CPosition& pos, int original_score, int8_t depth, uint8_t selectivity)
	//{
	//	const auto old_node_counter = m_search->NodeCount();
	//	const auto start_time = std::chrono::high_resolution_clock::now();

	//	const auto score = m_search->TestEval(pos, original_score, depth, selectivity);

	//	const auto end_time = std::chrono::high_resolution_clock::now();
	//	const auto duration = end_time - start_time;
	//	const auto node_count = m_search->NodeCount() - old_node_counter;

	//	m_logs.emplace_back(std::make_unique<CTestLog>(pos, depth, selectivity, original_score, score, node_count, duration));
	//	return score;
	//}

	//void oVerboseArchive::Caption()
	//{
	//	stream << " # | depth|score|       time (s) |      nodes (N) |    N/s     \n";
	//}

	//void oVerboseArchive::Header()
	//{
	//	Caption();
	//	Footer();
	//}

	//void oVerboseArchive::Footer()
	//{
	//	stream << "---+------+-----+----------------+----------------+------------\n";
	//}

	//void oVerboseArchive::Serialize(const ILog& log)
	//{

	//}

	//void oVerboseArchive::Serialize(const CTestLog& log)
	//{

	//}
}