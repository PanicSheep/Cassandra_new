#pragma once
#include "Search.h"
#include "Statistics.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <ostream>
#include <ratio>

namespace Search
{
	class CCliArchive : public oArchive
	{
	protected:
		std::ostream& stream;
		std::size_t node_count = 0;
		std::chrono::nanoseconds duration{ 0 };
		mutable std::recursive_mutex m_mtx;
	public:
		CCliArchive(std::ostream& stream) : stream(stream) {}

		void Header() override {}
		void Footer() override;
		void Serialize(const ILog::CEntry&) override;
	};

	class CVerboseCliArchive : public CCliArchive
	{
	public:
		CVerboseCliArchive(std::ostream& stream) : CCliArchive(stream) {}

		void Header() override;
		void Footer() override;
		void Serialize(const ILog::CEntry&) override;
	private:
		void Separator();
	};
	class CCliTestArchive : public oArchive
	{
	protected:
		std::ostream& stream;
		std::size_t node_count = 0;
		std::chrono::nanoseconds duration{ 0 };
		mutable std::recursive_mutex m_mtx;
		std::vector<int> score_diff;

	public:
		CCliTestArchive(std::ostream& stream) : stream(stream) {}

		void Header() override {}
		void Footer() override;
		void Serialize(const ILog::CEntry&) override;
	};

	class CVerboseCliTestArchive : public CCliTestArchive
	{
	public:
		CVerboseCliTestArchive(std::ostream& stream) : CCliTestArchive(stream) {}

		void Header() override;
		void Footer() override;
		void Serialize(const ILog::CEntry&) override;
	private:
		void Separator();
	};

	std::unique_ptr<Search::oArchive> CreateArchive(bool test, bool verbose, std::ostream& stream);
}