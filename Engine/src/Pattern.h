#pragma once
#include "Utility.h"
#include "Path.h"
#include "Position.h"
#include "Engine.h"
#include <array>
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace Pattern
{
	class CSumPow3Cache
	{
		std::array<uint32_t, (1ui64 << 15)> m_cache;
	public:
		CSumPow3Cache();
		uint64_t SumPow3(uint64_t exp) const;
	};
		
	uint32_t FullIndex(const CPosition&, const uint64_t pattern);

	namespace Configurations
	{
		class CBase
		{
		public:
			CBase(uint64_t pattern) : Pattern(pattern) {}
			virtual ~CBase() {}

			virtual std::vector<uint32_t> Configurations(const CPosition&) const = 0;
			virtual uint32_t Configuration(const CPosition&, std::size_t index) const = 0;
			virtual std::vector<uint64_t> Patterns() const = 0;

			virtual uint32_t ReducedSize() const = 0;
			virtual uint32_t Occurrences() const = 0; // TODO: This is Configurations(any).size().

			const uint64_t Pattern;
		};

		class CHorizontalSymmetric : public CBase
		{
			static inline uint64_t HALF = 0x0F0F0F0F0F0F0F0Fui64;
			static inline uint64_t MID = 0x0000001818000000ui64;
			const uint64_t m_patternC, m_patternV, m_patternD;
			const uint32_t m_half_size;

		public:
			CHorizontalSymmetric(uint64_t pattern);

			std::vector<uint32_t> Configurations(const CPosition&) const override;
			uint32_t Configuration(const CPosition&, std::size_t index) const override;
			std::vector<uint64_t> Patterns() const override;

			uint32_t ReducedSize() const override;
			uint32_t Occurrences() const override;
		private:
			uint32_t ReducedIndex0(CPosition) const;
			uint32_t ReducedIndex1(CPosition) const;
			uint32_t ReducedIndex2(CPosition) const;
			uint32_t ReducedIndex3(CPosition) const;
		};

		class CDiagonalSymmetric : public CBase
		{
			static const uint64_t HALF = 0x0080C0E0F0F8FCFEui64;
			static const uint64_t DIAG = 0x8040201008040201ui64;
			static const uint64_t MID = 0x0000001818000000ui64;
			const uint64_t m_patternH, m_patternC, m_patternV;
			const uint32_t m_half_size, m_diag_size;

		public:
			CDiagonalSymmetric(uint64_t pattern);

			std::vector<uint32_t> Configurations(const CPosition&) const override;
			uint32_t Configuration(const CPosition&, std::size_t index) const override;
			std::vector<uint64_t> Patterns() const override;

			uint32_t ReducedSize() const override;
			uint32_t Occurrences() const override;
		private:
			uint32_t ReducedIndex0(CPosition) const;
			uint32_t ReducedIndex1(CPosition) const;
			uint32_t ReducedIndex2(CPosition) const;
			uint32_t ReducedIndex3(CPosition) const;
		};

		class CAsymmetric : public CBase
		{
			static inline uint64_t MID = 0x0000001818000000ui64;
			const uint64_t m_patternH, m_patternV, m_patternD, m_patternC, m_patternHV, m_patternHD, m_patternHC;

		public:
			CAsymmetric(uint64_t pattern);

			std::vector<uint32_t> Configurations(const CPosition&) const override;
			uint32_t Configuration(const CPosition&, std::size_t index) const override;
			std::vector<uint64_t> Patterns() const override;

			uint32_t ReducedSize() const override;
			uint32_t Occurrences() const override;
		private:
			uint32_t ReducedIndex0(CPosition) const;
			uint32_t ReducedIndex1(CPosition) const;
			uint32_t ReducedIndex2(CPosition) const;
			uint32_t ReducedIndex3(CPosition) const;
			uint32_t ReducedIndex4(CPosition) const;
			uint32_t ReducedIndex5(CPosition) const;
			uint32_t ReducedIndex6(CPosition) const;
			uint32_t ReducedIndex7(CPosition) const;
		};

		std::unique_ptr<CBase> CreatePattern(uint64_t pattern);
	}

	namespace Eval
	{
		using CWeights = std::vector<float>;

		void For_each_config(const uint64_t pattern, std::function<void(CPosition)>);

		class CBase : public IEvaluator
		{
		public:
			CBase(uint64_t pattern) : Pattern(pattern) {}

			virtual float Eval(const CPosition&) const = 0;

			const uint64_t Pattern;
		};

		class CHorizontalSymmetric : public CBase
		{
			const uint64_t m_patternC, m_patternV, m_patternD;
			CWeights m_w0, m_w1, m_w2, m_w3;
		public:
			CHorizontalSymmetric(uint64_t pattern, std::vector<CWeights>);

			float Eval(const CPosition& pos) const override;
		};

		class CDiagonalSymmetric : public CBase
		{
			const uint64_t m_patternH, m_patternC, m_patternV;
			CWeights m_w0, m_w1, m_w2, m_w3;
		public:
			CDiagonalSymmetric(uint64_t pattern, std::vector<CWeights>);

			float Eval(const CPosition& pos) const override;
		};

		class CAsymmetric : public CBase
		{
			const uint64_t m_patternH, m_patternV, m_patternD, m_patternC, m_patternHV, m_patternHD, m_patternHC;
			CWeights m_w0, m_w1, m_w2, m_w3, m_w4, m_w5, m_w6, m_w7;
		public:
			CAsymmetric(uint64_t pattern, std::vector<CWeights>);

			float Eval(const CPosition& pos) const override;
		};

		std::unique_ptr<CBase> CreatePattern(uint64_t pattern, std::vector<CWeights>);
		std::unique_ptr<CBase> CreatePattern(uint64_t pattern, const CWeights& compressed);


		// Patterns in a pack always evaluate together.
		class CPack : public IEvaluator
		{
			std::vector<std::unique_ptr<CBase>> m_pack;
		public:
			CPack() {}
			CPack(std::size_t size) : m_pack(size) {}
			CPack(const std::vector<std::pair<uint64_t, CWeights>>&);

			void Add(std::unique_ptr<CBase>&&);
			std::size_t size() const;

			std::vector<uint64_t> Pattern() const;

			float Eval(const CPosition&) const;
		};

		// Holds a Pack for every 'empty_count'.
		class CEnsemble : public IEvaluator
		{
			std::array<std::shared_ptr<CPack>, 61> m_packs;
		public:
			void Set(uint8_t empty_count, std::shared_ptr<CPack>);

			float Eval(const CPosition&) const override;
		};
	}
}