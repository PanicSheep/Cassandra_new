#include "VerboseCliArchive.h"
#include "Puzzles.h"
#include "Utility.h"
#include <iomanip>

using namespace Search;

void CCliArchive::Footer()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);

	stream << ThousandsSeparator(node_count) << " nodes in " << time_format(duration)
		<< " (" << ThousandsSeparator(node_count * std::chrono::nanoseconds::period::den / duration.count()) << " N/s)" << std::endl;
}

void CCliArchive::Serialize(const ILog::CEntry& entry)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);

	node_count += entry.result.node_count;
	duration += entry.result.duration;
}

void CVerboseCliArchive::Separator()
{
	stream << "---+------+-----+----------------+----------------+------------\n";
}

void CVerboseCliArchive::Header()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);
	stream << " # | depth|score|       time (s) |      nodes (N) |    N/s     \n";
	Separator();
}

void CVerboseCliArchive::Footer()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);
	Separator();
	CCliArchive::Footer();
}

void CVerboseCliArchive::Serialize(const ILog::CEntry& entry)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);
	CCliArchive::Serialize(entry);

	stream << std::setw(3) << std::to_string(entry.index) << "|";
	stream << std::setw(6) << std::to_string(entry.specification.depth) << "|";
	stream << " " << DoubleDigitSignedInt(entry.result.score) << " |";
	stream << std::setw(16) << time_format(entry.result.duration) << "|";
	stream << std::setw(16) << ThousandsSeparator(entry.result.node_count) << "|";

	const auto nano_seconds = entry.result.duration.count();
	if (nano_seconds > 0)
		stream << std::setw(12) << ThousandsSeparator(static_cast<uint64_t>(static_cast<double>(entry.result.node_count) * std::nano::den / nano_seconds));
	stream << std::endl;
}

void CCliTestArchive::Footer()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);

	stream << ThousandsSeparator(node_count) << " nodes in " << time_format(duration)
		<< " (" << ThousandsSeparator(node_count * std::chrono::nanoseconds::period::den / duration.count()) << " N/s)\n";

	const auto correct = std::count_if(score_diff.begin(), score_diff.end(), [](int i) { return i == 0; });;
	stream << "Tests correct: " << correct << "\n";
	stream << "Tests wrong: " << score_diff.size() - correct << "\n";
	stream << "stddev(score_diff) = " << StandardDeviation(score_diff) << std::endl;
}

void CCliTestArchive::Serialize(const ILog::CEntry& entry)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);

	node_count += entry.result.node_count;
	duration += entry.result.duration;
	score_diff.push_back(entry.result.score - entry.original_score);
}

void CVerboseCliTestArchive::Separator()
{
	stream << "---+------+-----+-----+-----+----------------+----------------+------------\n";
}

void CVerboseCliTestArchive::Header()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);
	stream << " # | depth| eval|score| diff|       time (s) |      nodes (N) |    N/s     \n";
	Separator();
}

void CVerboseCliTestArchive::Footer()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);
	Separator();
	CCliTestArchive::Footer();
}

void CVerboseCliTestArchive::Serialize(const ILog::CEntry& entry)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtx);
	CCliTestArchive::Serialize(entry);

	stream << std::setw(3) << std::to_string(entry.index) << "|";
	stream << std::setw(6) << std::to_string(entry.specification.depth) << "|";
	stream << " " << DoubleDigitSignedInt(entry.result.score) << " |";
	stream << " " << DoubleDigitSignedInt(entry.original_score) << " |";
	if (entry.original_score == Puzzle::DEFAULT_SCORE)
		stream << "     |";
	else
		stream << " " << DoubleDigitSignedInt(entry.result.score - entry.original_score) << " |";
	stream << std::setw(16) << time_format(entry.result.duration) << "|";
	stream << std::setw(16) << ThousandsSeparator(entry.result.node_count) << "|";

	const auto nano_seconds = entry.result.duration.count();
	if (nano_seconds > 0)
		stream << std::setw(12) << ThousandsSeparator(static_cast<uint64_t>(static_cast<double>(entry.result.node_count) * std::nano::den / nano_seconds));
	stream << std::endl;
}

std::unique_ptr<Search::oArchive> Search::CreateArchive(bool test, bool verbose, std::ostream& stream)
{
	if (test)
	{
		if (verbose)
			return std::make_unique<Search::CVerboseCliTestArchive>(stream);
		else
			return std::make_unique<Search::CCliTestArchive>(stream);
	}
	else
	{
		if (verbose)
			return std::make_unique<Search::CVerboseCliArchive>(stream);
		else
			return std::make_unique<Search::CCliArchive>(stream);
	}
}
