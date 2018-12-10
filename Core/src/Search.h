#pragma once
#include "Position.h"
#include <cstdint>
#include <chrono>
#include <memory>

class Engine;

namespace Search
{
	static inline int infinity = +65;

	struct CSpecification
	{
		int8_t depth;
		uint8_t selectivity;

		CSpecification(int8_t depth, uint8_t selectivity);
		static CSpecification SolveExact(CPosition pos) { return CSpecification(pos.EmptyCount(), 0); }
	};

	struct CResult
	{
		int8_t score;
		std::size_t node_count;
		std::chrono::nanoseconds duration;

		CResult(int8_t score, std::size_t node_count, std::chrono::nanoseconds duration);
	};

	class CAlgorithm
	{
	protected:
		std::shared_ptr<Engine> engine;
		std::size_t node_counter = 0;

		static int EvalGameOver(const CPosition&); // TODO: Remove!
	public:
		CAlgorithm(const std::shared_ptr<Engine>&);

		virtual std::unique_ptr<CAlgorithm> Clone() const = 0;

		virtual CResult Eval(const CPosition&, CSpecification) = 0;
		virtual CResult Eval(const CPosition&);

		std::size_t NodeCount() const { return node_counter; }
	};

	class oArchive;

	struct ILog
	{
		struct CEntry
		{
			CPosition position;
			std::size_t index;
			int original_score;
			CSpecification specification;
			CResult result;

			CEntry(CPosition position, std::size_t index, int original_score, CSpecification specification, CResult result)
				: position(position), index(index), original_score(original_score), specification(specification), result(result)
			{}
		};
		std::size_t index;

		virtual std::unique_ptr<ILog> Clone() const = 0;
		virtual void push_back(CPosition, int original_score, CSpecification, CResult) = 0;
		virtual void Serialize(oArchive&) const = 0;
	};

	class oArchive
	{
	public:
		virtual ~oArchive() {}

		virtual void Header() {}
		virtual void Footer() {}
		virtual void Serialize(const ILog::CEntry&) = 0;
		oArchive& operator<<(const ILog& obj) { obj.Serialize(*this); return *this; }
	};

	class CLogNull : public ILog
	{
	public:
		std::unique_ptr<ILog> Clone() const override;
		void push_back(CPosition, int, CSpecification, CResult) override {}
		void Serialize(oArchive&) const override {}
	};

	class CLogCollector : public ILog
	{
		std::vector<CEntry> m_vec;
	public:
		CLogCollector() {}
		CLogCollector(const CLogCollector&);

		std::unique_ptr<ILog> Clone() const override;
		void push_back(CPosition pos, int original_score, CSpecification spec, CResult result) override { m_vec.push_back(CLogCollector::CEntry(pos, index, original_score, spec, result)); }
		void Serialize(oArchive& archive) const override { for (const auto& it : m_vec) archive.Serialize(it); }
	};

	class CLogPassThrough : public ILog
	{
		oArchive& m_archive;
	public:
		CLogPassThrough(oArchive& archive) : m_archive(archive) {}
		CLogPassThrough(const CLogPassThrough&);

		std::unique_ptr<ILog> Clone() const override;
		void push_back(CPosition pos, int original_score, CSpecification spec, CResult result) override { m_archive.Serialize(CLogCollector::CEntry(pos, index, original_score, spec, result)); }
		void Serialize(oArchive&) const override {}
	};
}